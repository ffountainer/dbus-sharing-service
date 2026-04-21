#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <random>
#include <sdbus-c++/sdbus-c++.h>
#include <sharing_service.hpp>
#include <string>
#include <vector>

void registerService(sdbus::MethodCall call) {

  std::string name;
  std::vector<std::string> supportedFormats;
  call >> name;
  call >> supportedFormats;

  if (name.empty() || supportedFormats.empty()) {
    throw sdbus::Error(
        sdbus::Error::Name{"com.system.Sharing.Error"},
        "RegisterService: Couldn't register service, the data is invalid");
  }

  nlohmann::json services = loadServiceTable("registerService");

  if (services.contains(name)) {
    throw sdbus::Error(
        sdbus::Error::Name{"com.system.Sharing.Error"},
        "RegisterService: The service has already been registered");
  } else {
    services[name] = supportedFormats;
  }

  service::updateServicesList(services);

  auto reply = call.createReply();

  reply.send();
}

void openFile(sdbus::MethodCall call) {

  std::string filePath;
  call >> filePath;

  openFileGeneral("openFile", filePath);

  auto reply = call.createReply();
  reply.send();
}

void openFileUsingService(sdbus::MethodCall call) {

  std::string path;
  std::string service;
  call >> path;
  call >> service;

  openFileGeneral("openFileUsingService", path, service);

  auto reply = call.createReply();
  reply.send();
}
