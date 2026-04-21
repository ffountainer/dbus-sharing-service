#ifndef LOAD_H
#define LOAD_H

#include <nlohmann/json.hpp>
#include <string>

extern std::string g_path;

nlohmann::json loadServiceTable(std::string caller, std::string extension = "");

#endif