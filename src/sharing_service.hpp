#ifndef SHARING_SERVICE_HPP
#define SHARING_SERVICE_HPP

#include <sdbus-c++/sdbus-c++.h>
#include <nlohmann/json.hpp>
#include <string>

extern std::string g_path = "./.config/services-list.json";

nlohmann::json loadServiceTable(const std::string& caller,
                                const std::string& extension = "",
                                const std::string& service = "");

void openFileGeneral(const std::string& caller,
                     const std::string& filePath,
                     const std::string& service = "");

namespace service {

void registerService(sdbus::MethodCall call);
void openFile(sdbus::MethodCall call);
void openFileUsingService(sdbus::MethodCall call);
void updateServicesList(nlohmann::json updated);
}

#endif