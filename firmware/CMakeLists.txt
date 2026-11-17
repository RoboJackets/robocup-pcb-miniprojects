cmake_minimum_required(VERSION 3.13)

# ON = regular Pico for dev, OFF = our PCB. Has to be set before the SDK import.
option(FULGORA_DEV_PICO "Build for a stock Raspberry Pi Pico instead of the Fulgora PCB" ON)

if(FULGORA_DEV_PICO)
    set(PICO_BOARD pico CACHE STRING "Board type")
else()
    list(APPEND PICO_BOARD_HEADER_DIRS ${CMAKE_CURRENT_LIST_DIR}/boards)
    set(PICO_BOARD fulgora CACHE STRING "Board type")
endif()

include(pico_sdk_import.cmake)

project(fulgora_firmware C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(lib/tca9534)

add_executable(fulgora
    src/main.cpp
    src/i2c_bus.cpp
    src/daisy_chain.cpp
)

target_compile_options(fulgora PRIVATE -Wall -Wextra)

# test mode: board makes its own packets (for Pico loopback / two-Pico tests)
option(FULGORA_SPI_TEST_SOURCE "Generate test packets on SPI1 instead of forwarding" OFF)
if(FULGORA_SPI_TEST_SOURCE)
    target_compile_definitions(fulgora PRIVATE FULGORA_SPI_TEST_SOURCE=1)
endif()

target_include_directories(fulgora PRIVATE src)

target_link_libraries(fulgora
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_dma
    pico_sync
    tca9534
)

# printf over USB only, the default UART pins (GPIO0/1) are SPI0 on our board
pico_enable_stdio_usb(fulgora 1)
pico_enable_stdio_uart(fulgora 0)

pico_add_extra_outputs(fulgora)
