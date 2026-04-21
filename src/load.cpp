#include <algorithm>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <random>
#include <sdbus-c++/sdbus-c++.h>

std::string g_path = "./.config/services-list.json";

nlohmann::json loadServiceTable(std::string caller,
                                std::string extension = "") {

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

  if (caller == "openFile") {
    if (!std::filesystem::exists(g_path)) {
      throw sdbus::Error(sdbus::Error::Name{"com.system.Sharing.Error"},
                         "OpenFile: There is no service available!");
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
                         std::string("OpenFile: Service Table is invalid"));
    }
    table.close();
  }

  if (!extension.empty()) {

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

  return services;
}