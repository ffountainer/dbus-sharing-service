#ifndef SHARING_SERVICE_HPP
#define SHARING_SERVICE_HPP

#include <nlohmann/json.hpp>
#include <sdbus-c++/sdbus-c++.h>
#include <string>

namespace service {

void registerService(sdbus::MethodCall call);
void openFile(sdbus::MethodCall call);
void openFileUsingService(sdbus::MethodCall call);
void updateServicesList(nlohmann::json updated);
nlohmann::json loadServiceTable(const std::string &caller,
                                const std::string &extension = "",
                                const std::string &service = "");

void openFileGeneral(const std::string &caller, const std::string &filePath,
                     const std::string &service = "");

void updateServicesList(nlohmann::json updated);
} // namespace service

#endif