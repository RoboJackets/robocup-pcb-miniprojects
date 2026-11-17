# TCA9534 driver
# to use it: add_subdirectory(path/to/tca9534) then link against tca9534

add_library(tca9534 STATIC
    tca9534.cpp
)

target_include_directories(tca9534 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_options(tca9534 PRIVATE -Wall -Wextra)

target_link_libraries(tca9534 PUBLIC
    pico_stdlib
    hardware_i2c
)
