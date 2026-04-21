cmake_minimum_required(VERSION 3.10.0)
project(dbus-sharing-service VERSION 0.1.0 LANGUAGES C CXX)

add_executable(dbus-sharing-service src/service.cpp src/main.cpp src/logic.cpp)

target_compile_features(dbus-sharing-service PRIVATE cxx_std_20)
target_compile_options(dbus-sharing-service PRIVATE -fcoroutines)
target_include_directories(dbus-sharing-service PRIVATE src)
find_package(sdbus-c++ REQUIRED)
target_link_libraries(dbus-sharing-service PRIVATE sdbus-c++)
find_package(nlohmann_json REQUIRED)
target_link_libraries(dbus-sharing-service PRIVATE nlohmann_json::nlohmann_json)