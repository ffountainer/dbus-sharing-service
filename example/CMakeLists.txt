add_executable(example-service main.cpp)

target_include_directories(example-service PRIVATE
    ../lib/include
)

target_link_libraries(example-service
    PRIVATE
        dbussharing
)