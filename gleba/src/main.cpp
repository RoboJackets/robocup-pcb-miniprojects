// Gleba firmware entry point.
// Core 0: SPI daisy chain (core0.cpp). Core 1: IMU + buttons/LEDs (core1.cpp).

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "core0.hpp"
#include "core1.hpp"
#include "shared.hpp"

int main() {
    stdio_init_all();                    // USB serial for printf debugging

    shared_init();                       // 1. create the mutex first
    multicore_launch_core1(core1_main);  // 2. then start core 1

    core0_init();                        // 3. core 0 sets up SPI and runs the chain
    core0_main();                        //    (never returns)
}