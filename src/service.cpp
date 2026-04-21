#include <filesystem>
#include <fstream>
#include <load.h>
#include <nlohmann/json.hpp>
#include <random>
#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <vector>

void registerService(sdbus::MethodCall call) {

  std::string name;
  std::vector<std::string> supportedFormats;
  call >> name;
  call >> supportedFormats;

  nlohmann::json services = loadServiceTable("registerService");

  if (services.contains(name)) {
    throw sdbus::Error(
        sdbus::Error::Name{"com.system.Sharing.Error"},
        "RegisterService: The service has already been registered");
  } else {
    services[name] = supportedFormats;
  }

  std::ofstream newTable(g_path, std::ios::trunc);

  newTable << services.dump(2);
  newTable.close();

  auto reply = call.createReply();

  reply.send();
}

void openFile(sdbus::MethodCall call) {

  std::string filePath;
  call >> filePath;

  if (!std::filesystem::exists(filePath)) {
    throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                       "OpenFile: requested file does not exist!!!!");
  }

  // here we are extracting extension from the path
  std::filesystem::path file(filePath);
  std::string extension = file.extension().string();
  if (!extension.empty() && extension[0] == '.')
    extension.erase(0, 1);

  // now we want to open the file with the list of registered services

  nlohmann::json services = loadServiceTable("openFile", extension);

  std::string availableService = services["openWith"];

  try {
    sdbus::ServiceName service{availableService};
    sdbus::ObjectPath path{"/"};
    auto proxy = sdbus::createProxy(service, path);

    proxy->callMethod("OpenFile")
        .onInterface(availableService)
        .withArguments(filePath);
  } catch (sdbus::Error &error) {
    throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                       std::string("OpenFile: could not open the file") +
                           error.getMessage());
  }

  auto reply = call.createReply();
  reply.send();
}

void openFileUsingService(sdbus::MethodCall call) {

  std::string path;
  std::string service;
  call >> path;
  call >> service;
}

int main(int, char **) {

  // here I initialise the service with the name of connection
  sdbus::ServiceName serviceName{"com.system.sharing"};
  // and create a connection itself
  auto connection = sdbus::createSessionBusConnection(serviceName);

  sdbus::ObjectPath objectPath{"/"};

  // pointer to the object
  auto sharingSystem = sdbus::createObject(*connection, std::move(objectPath));

  sdbus::InterfaceName sharingInterface{"com.system.Sharing"};

  sharingSystem
      ->addVTable(
          sdbus::MethodVTableItem{sdbus::MethodName{"registerService"},
                                  sdbus::Signature{"sas"},
                                  {},
                                  {},
                                  {},
                                  &registerService,
                                  {}},
          sdbus::MethodVTableItem{sdbus::MethodName{"openFile"},
                                  sdbus::Signature{"s"},
                                  {},
                                  {},
                                  {},
                                  &openFile,
                                  {}},
          sdbus::MethodVTableItem{sdbus::MethodName{"openFileUsingService"},
                                  sdbus::Signature{"ss"},
                                  {},
                                  {},
                                  {},
                                  &openFileUsingService,
                                  {}})
      .forInterface(sharingInterface);

  connection->enterEventLoop();
}
