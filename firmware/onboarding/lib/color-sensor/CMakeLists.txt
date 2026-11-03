cmake_minimum_required(VERSION 3.13)

# Only declare a project if someone configures this folder directly.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(color_sensor_driver LANGUAGES C CXX ASM)
    message(FATAL_ERROR
        "color_sensor_driver is a library, not a standalone project.\n"
        "Build your main Pico project instead; it pulls this in with add_subdirectory().")
endif()

if(NOT TARGET pico_stdlib)
    message(FATAL_ERROR
        "color_sensor_driver: pico_stdlib not found. "
        "add_subdirectory() this AFTER pico_sdk_init() in the parent CMakeLists.")
endif()

# INTERFACE library: sources compile inside the executable's target,
# so SDK sources aren't duplicated into this library.
add_library(color_sensor_driver INTERFACE)

target_sources(color_sensor_driver INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/color_sensor.cpp
)

target_include_directories(color_sensor_driver INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_compile_features(color_sensor_driver INTERFACE cxx_std_17)

target_link_libraries(color_sensor_driver INTERFACE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)