#include "spi.hpp"
#include "hardware/spi.h"
#include "pico/stdlib.h"

void receive() {
    spi_read_blocking(spi0, 0, in_buf, CLUB_BUF_LEN); 
}

void copy_over() {
    for (int i = 0; i < CLUB_BUF_LEN; i++) {
        if (i < 14 || i > 19) {
            out_buf[i] = in_buf[i]; 
        }
    } 
    // Let's lock down output buffer and read from the data
    mutex_enter_blocking(&output_mutex);
    for (int i = 0; i < 6; i++) {
        out_buf[i] = data_buf[i];
    }
    mutex_exit(&output_mutex);
}

void transmit() {
    spi_write_blocking(spi1, out_buf, CLUB_BUF_LEN); 
}

void spi_handle_interrupt(uint gpio, uint32_t events) {
    if (gpio == R_SS && (events & GPIO_IRQ_EDGE_FALL)) {
        receive(); 
        copy_over(); 
        transmit(); 
    }
}

void spi_init() {
    spi_init(spi1, 1000*1000*FREQ);
    spi_set_format(spi1, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    gpio_set_function(S_RX, GPIO_FUNC_SPI);
    gpio_set_function(S_TX, GPIO_FUNC_SPI);
    gpio_set_function(S_CLK, GPIO_FUNC_SPI);
    gpio_set_function(S_SS, GPIO_FUNC_SPI);

    spi_init(spi0, 1000*1000*FREQ);
    spi_set_format(spi0, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    gpio_set_function(R_RX, GPIO_FUNC_SPI);
    gpio_set_function(R_TX, GPIO_FUNC_SPI);
    gpio_set_function(R_CLK, GPIO_FUNC_SPI);
    gpio_set_function(R_SS, GPIO_FUNC_SPI);

    for (uint8_t i = 0; i < CLUB_BUF_LEN; i++) {
        in_buf[i] = 0;
        out_buf[i] = 0; 
    }

    // Interrupt on R_SS pin pulled low 
    // "GET FUCKING SET" 
    // -- Matt Ryan
    gpio_set_irq_enabled_with_callback(R_SS, GPIO_IRQ_EDGE_FALL, true, &spi_handle_interrupt); 
}

