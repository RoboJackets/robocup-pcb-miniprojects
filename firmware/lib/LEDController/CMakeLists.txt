cmake_minimum_required(VERSION 3.13)

# Only declare a project if someone configures this folder directly.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(LEDController LANGUAGES C CXX ASM)
    message(FATAL_ERROR
        "LEDController is a library, not a standalone project.\n"
        "Build your main Pico project instead; it pulls this in with add_subdirectory().")
endif()

if(NOT TARGET pico_stdlib)
    message(FATAL_ERROR
        "LEDController: pico_stdlib not found. "
        "add_subdirectory() this AFTER pico_sdk_init() in the parent CMakeLists.")
endif()

# INTERFACE library: sources compile inside the executable's target,
# so SDK sources aren't duplicated into this library.
add_library(LEDController INTERFACE)

target_sources(LEDController INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/LEDController.cpp
)

target_include_directories(LEDController INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_features(LEDController INTERFACE cxx_std_17)

target_link_libraries(LEDController INTERFACE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)