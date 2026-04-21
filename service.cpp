#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <vector>
#include <random>

std::string g_path = "./.config/services-list.json";

nlohmann::json loadServiceTable(std::string caller,
                                std::string extension = "") {

  nlohmann::json services;
  std::fstream table;

  if (!std::filesystem::exists("./.config")) {
    std::filesystem::create_directories("./.config");
  }

  if (caller.compare("registerService")) {

    if (!std::filesystem::exists(g_path)) {
      std::fstream newFile(g_path);
      newFile << "{}";
    }

    std::fstream table(g_path, std::ios::in | std::ios::out);
  }

  if (caller.compare("openFile")) {
    if (!std::filesystem::exists(g_path)) {
      throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                         "OpenFile: There is no service available!");
    }
    std::fstream table(g_path, std::ios::in);
  }

  if (!table.is_open()) {
    throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                       "Cannot access registered services file :((");
  } else {
    table >> services;
    table.close();
  }

  if (!extension.compare("")) {

    std::vector<std::string> available;
    for (std::string key : services) {
      std::vector<std::string> supported = services[key];
      int availableServices = count(key.begin(), key.end(), extension);
      if (availableServices > 0) {
        available.insert(available.end(), key);
      }
    }
    if (available.size() == 0) {
      throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                       "OpenFile: there is no service that can open your file :(");
    }

    std::random_device rd;
    std::mt19937 eng(rd());  // seed 
    std::uniform_int_distribution<> distr(0, available.size() - 1);
    
    int randomIndex = distr(eng);

    nlohmann::json returnJson;
    returnJson["openWith"] = available[randomIndex];

    services = returnJson;

  }
  
  return services;
}

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

  auto reply = call.createReply();

  reply.send();
}

void openFile(sdbus::MethodCall call) {

  std::string path;
  call >> path;

  // here we are extracting extension from the path
  std::filesystem::path file(path);
  std::string extension = file.extension().string();

  // now we want to open the file with the list of registered services

  nlohmann::json services = loadServiceTable("openFile", extension);

  std::string availableService = services["openWith"];

  try {
    sdbus::ServiceName service{availableService};
    sdbus::ObjectPath path{"/"};
    auto proxy = sdbus::createProxy(service, path);

    proxy->callMethod("OpenFile")
         .onInterface(availableService)
         .withArguments(path);
  }
  catch (sdbus::Error& error) {
    throw sdbus::Error(
        sdbus::Error::Name{"com.system.Sharing.Error"},
        std::string("OpenFile: could not open the file") + error.getMessage());
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

  sdbus::ObjectPath objectPath{"/com/system/sharing"};

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
