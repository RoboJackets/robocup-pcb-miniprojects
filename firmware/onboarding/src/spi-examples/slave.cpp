//=================================================================================//

#include <stdio.h>
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

//=================================================================================//

#define BUF_LEN 128

//=================================================================================//

int main() {
  // Enable UART so we can print
  stdio_init_all();
  sleep_ms (2 * 1000);
  printf ("SPI Peripheral Example\n");

  // Enable SPI 0 at 1 MHz and connect to GPIOs
  spi_init (spi_default, 1 * 1000000);
  spi_set_slave (spi_default, true);

    gpio_set_function(16, GPIO_FUNC_SPI);  // RX
    gpio_set_function(17, GPIO_FUNC_SPI);  // CSn
    gpio_set_function(18, GPIO_FUNC_SPI);  // SCK
    gpio_set_function(19, GPIO_FUNC_SPI);  // TX

  uint8_t out_buf [BUF_LEN], in_buf [BUF_LEN];

  // Initialize output buffer
  for (uint8_t i = 0; i < BUF_LEN; ++i) {
    out_buf [i] = 0;
    in_buf [i] = 0;
  }

  while (1) {
    if (spi_is_readable (spi_default)) {
      printf ("Reading data from SPI..\n");
      // Write the output buffer to MOSI, and at the same time read from MISO.
      spi_read_blocking (spi_default, 0, in_buf, 1);

      printf ("Data received: %d\n", in_buf [0]);
    }
  }
}

//=================================================================================//
