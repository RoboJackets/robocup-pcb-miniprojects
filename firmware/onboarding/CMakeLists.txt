cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
set(USERHOME $ENV{USERPROFILE})
else()
set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(onboarding C CXX ASM)

pico_sdk_init()

add_subdirectory(lib/hall-effect-sensor)
add_subdirectory(lib/color-sensor)

add_executable(onboarding 
    src/main.cpp 
    src/spi/spi.cpp
)

pico_set_program_name(onboarding "onboarding")
pico_set_program_version(onboarding "0.1")

# pico_generate_pio_header(onboarding ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

pico_enable_stdio_uart(onboarding 0)
pico_enable_stdio_usb(onboarding 1)

target_link_libraries(onboarding
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_dma
    hardware_pio
    hardware_interp
    hardware_timer
    hardware_watchdog
    hardware_clocks
    pico_multicore
    hall_effect_driver
    color_sensor_driver
)

target_include_directories(onboarding PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(onboarding)