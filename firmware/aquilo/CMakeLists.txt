# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(aquilo C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# add the TempSensor directory to our project
add_subdirectory(lib/TempSensor)

# Add executable. Default name is the project name, version 0.1

add_executable(aquilo aquilo.c)

pico_set_program_name(aquilo "aquilo")
pico_set_program_version(aquilo "0.1")

# Generate PIO header
pico_generate_pio_header(aquilo ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(aquilo 1)
pico_enable_stdio_usb(aquilo 0)

# Add the standard library to the build
target_link_libraries(aquilo
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(aquilo PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(aquilo 
        hardware_spi
        hardware_i2c
        hardware_pio
	# include the library defined in the TempSensor directory
	TempSensor
        )

pico_add_extra_outputs(aquilo)

