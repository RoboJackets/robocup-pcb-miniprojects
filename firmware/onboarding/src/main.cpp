#include <stdio.h>
#include "pico/multicore.h" 
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "spi/spi.hpp"

uint8_t output[CLUB_BUF_LEN]; 

void core_1_setup() {
  spi_init(); 
}

void core_0_setup() {
  
}

/**
 * @brief Core 1 loop -- Will be used for SPI 
 * 
 */
void core_1_main() {

}

void core_0_main() {

}

int main() {
  stdio_init_all();
  sleep_ms(10); 

  core_1_setup();
  core_0_setup(); 

  multicore_launch_core1(&core_1_main); 

  while (1) {
    core_0_main(); 
  }
}