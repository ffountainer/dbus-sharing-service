#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <vector>

std::string g_path = "./.config/services-list.json";

void registerService(sdbus::MethodCall call) {

  std::string name;
  std::vector<std::string> supportedFormats;
  call >> name;
  call >> supportedFormats;

  if (!std::filesystem::exists("./.config")) {
    std::filesystem::create_directories("./.config");
  }

  if (!std::filesystem::exists(g_path)) {
    std::fstream newFile(g_path);
    newFile << "{}";
  }

  std::fstream table(g_path, std::ios::in | std::ios::out);

  nlohmann::json services;

  if (!table.is_open()) {
    throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                       "File cannot be opened :((");
  } else {
    table >> services;
    table.close();
  }

  if (services.contains(name)) {
    std::cout << "The service has already been registered";
  } else {
    services[name] = supportedFormats;
  }

  std::ofstream newTable(g_path, std::ios::trunc);

  newTable << services.dump(2);

  auto reply = call.createReply();

  reply.send();
}

void openFile(sdbus::MethodCall call) {

  std::string path;
  call >> path;
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

  sdbus::ObjectPath objectPath{"/com/system/sharing"};

  auto sharingSystem = sdbus::createObject(*connection, std::move(objectPath));

  // pointer to the object

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
