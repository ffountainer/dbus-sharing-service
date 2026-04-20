cmake_minimum_required(VERSION 3.10.0)
project(dbus-sharing-service VERSION 0.1.0 LANGUAGES C CXX)

add_executable(dbus-sharing-service main.cpp)

target_compile_features(dbus-sharing-service PRIVATE cxx_std_20)
target_compile_options(dbus-sharing-service PRIVATE -fcoroutines)
find_package(sdbus-c++ REQUIRED)
target_link_libraries(dbus-sharing-service PRIVATE sdbus-c++)