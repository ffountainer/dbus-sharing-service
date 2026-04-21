#include <algorithm>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <random>
#include <sdbus-c++/sdbus-c++.h>

std::string g_path = "./.config/services-list.json";

nlohmann::json loadServiceTable(std::string caller, std::string extension = "",
                                std::string service = "") {

  nlohmann::json services;
  std::fstream table;

  if (!std::filesystem::exists("./.config")) {
    std::filesystem::create_directories("./.config");
  }

  if (caller == "registerService") {

    if (!std::filesystem::exists(g_path)) {
      std::fstream newFile(g_path);
      newFile << "{}";
      newFile.close();
    }

    table.open(g_path, std::ios::in | std::ios::out);
  }

  if (caller == "openFile" || caller == "openFileUsingService") {
    if (!std::filesystem::exists(g_path)) {
      throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                         caller + ": There is no service available!");
    }
    table.open(g_path, std::ios::in);
  }

  if (!table.is_open()) {
    throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                       "Cannot access registered services file :((");
  } else {
    try {
      table >> services;
    } catch (nlohmann::json::exception &error) {
      throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                         std::string(caller + ": Service Table is invalid"));
    }
    table.close();
  }

  if (!extension.empty() && caller == "openFile") {

    std::vector<std::string> available;
    for (auto &[key, value] : services.items()) {
      std::vector<std::string> supported =
          value.get<std::vector<std::string>>();
      if (std::find(supported.begin(), supported.end(), extension) !=
          supported.end()) {
        available.push_back(key);
      }
    }
    if (available.size() == 0) {
      throw sdbus::Error(
          sdbus::Error::Name{"com.system.Sharing.Error"},
          "OpenFile: there is no service that can open your file :(");
    }

    std::random_device rd;
    std::mt19937 eng(rd()); // seed
    std::uniform_int_distribution<> distr(0, available.size() - 1);

    int randomIndex = distr(eng);

    nlohmann::json returnJson;
    returnJson["openWith"] = available[randomIndex];

    services = returnJson;
  }

  if (!extension.empty() && caller == "openFileUsingService") {
    if (!services.contains(service)) {
      throw sdbus::Error(
          sdbus::Error::Name{"com.system.Sharing.Error"},
          "OpenFileUsingService: provided service is not available!");
    }
    std::vector<std::string> supported =
        services[service].get<std::vector<std::string>>();
    if (std::find(supported.begin(), supported.end(), extension) !=
        supported.end()) {
      nlohmann::json returnJson;
      if (!services.contains("openWith")) {
        throw sdbus::Error(
            sdbus::Error::Name{"com.system.Sharing.Error"},
            caller + ": error while processing file: no service is available");
      }
      returnJson["openWith"] = service;
      services = returnJson;
    } else {
      throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                         "OpenFileUsingService: files with extension " +
                             extension +
                             " are not supported by requested service!");
    }
  }
  return services;
}

void openFileGeneral(std::string caller, std::string filePath,
                     std::string service = "") {

  if (!std::filesystem::exists(filePath)) {
    throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                       caller + ": requested file does not exist!!!!");
  }

  // here we are extracting extension from the path
  std::filesystem::path file(filePath);
  std::string extension = file.extension().string();
  if (!extension.empty() && extension[0] == '.')
    extension.erase(0, 1);

  // now we want to open the file with the list of registered services

  nlohmann::json services;

  if (caller == "openFile") {
    services = loadServiceTable(caller, extension);
  }

  if (caller == "openFileUsingService") {
    services = loadServiceTable(caller, extension, service);
  }

  std::string availableService = services["openWith"];

  try {
    sdbus::ServiceName processingService{availableService};
    sdbus::ObjectPath path{"/"};
    auto proxy = sdbus::createProxy(processingService, path);

    proxy->callMethod("OpenFile")
        .onInterface(availableService)
        .withArguments(filePath);
  } catch (sdbus::Error &error) {
    throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                       std::string(caller + ": could not open the file") +
                           error.getMessage());
  }
}