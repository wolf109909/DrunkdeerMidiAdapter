cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 23)

project(DrunkDeer-Midi VERSION 0.1.0 LANGUAGES C CXX)
add_subdirectory(thirdparty/hidapi)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/libremidi)
add_subdirectory(thirdparty/json)

add_executable(
    DrunkDeer-Midi 
    "main.cpp"
    "include/libdd/libdd.h"
    "include/libdd/layouts.h"
    "include/libdd/packets.h"
    "include/libdd/encodings.h"
    )
target_link_libraries(DrunkDeer-Midi PRIVATE libremidi spdlog hidapi nlohmann_json::nlohmann_json)
