cmake_minimum_required(VERSION 3.13)

# Only declare a project if someone configures this folder directly.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(hall_effect_driver LANGUAGES C CXX ASM)
    message(FATAL_ERROR
        "hall_effect_driver is a library, not a standalone project.\n"
        "Build your main Pico project instead; it pulls this in with add_subdirectory().")
endif()

if(NOT TARGET pico_stdlib)
    message(FATAL_ERROR
        "hall_effect_driver: pico_stdlib not found. "
        "add_subdirectory() this AFTER pico_sdk_init() in the parent CMakeLists.")
endif()

# INTERFACE library: sources compile inside the executable's target,
# so SDK sources aren't duplicated into this library.
add_library(hall_effect_driver INTERFACE)

target_sources(hall_effect_driver INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/hall_effect.cpp
)

target_include_directories(hall_effect_driver INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_compile_features(hall_effect_driver INTERFACE cxx_std_17)

target_link_libraries(hall_effect_driver INTERFACE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)