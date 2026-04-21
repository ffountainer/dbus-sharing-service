project(dbussharing
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dbussharing SHARED
    src/dbussharing.cpp
)

target_include_directories(dbussharing
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_compile_options(dbussharing PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)


find_package(PkgConfig REQUIRED)
pkg_check_modules(SDBUSCPP REQUIRED sdbus-c++)

target_include_directories(dbussharing PRIVATE ${SDBUSCPP_INCLUDE_DIRS})
target_link_directories(dbussharing PRIVATE ${SDBUSCPP_LIBRARY_DIRS})
target_link_libraries(dbussharing PRIVATE ${SDBUSCPP_LIBRARIES})
target_link_libraries(dbussharing PUBLIC sdbus-c++)


find_package(Threads REQUIRED)
target_link_libraries(dbussharing PRIVATE Threads::Threads)


set_target_properties(dbussharing PROPERTIES
    OUTPUT_NAME "dbussharing"
)