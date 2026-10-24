#include "spi.hpp"
#include "hardware/spi.h"
#include "pico/stdlib.h"

SPI::SPI(uint8_t RX, uint8_t TX, uint8_t CLK, uint8_t SS, size_t buf_length) : buf_length(buf_length) {
    this->in_buf = new uint8_t[buf_length];
    this->out_buf = new uint8_t[buf_length]; 
    this->RX = RX;
    this->TX = TX;
    this->CLK = CLK;
    this->SS = SS; 
}

int SPI::init(bool prod) {
    // Initalize SPI0 at 1 MHz
    spi_init(spi_default, 1000*1000); 

    // Set pins to SPI
    gpio_set_function(this->RX, GPIO_FUNC_SPI);
    gpio_set_function(this->TX, GPIO_FUNC_SPI);
    gpio_set_function(this->CLK, GPIO_FUNC_SPI);
    gpio_set_function(this->SS, GPIO_FUNC_SPI);

    if(prod){
        // Interrupt on MISO pin read
        set_interrupt(this->RX, GPIO_IRQ_EDGE_RISE);

    }

    return 0; 
}

// Test just writes with poll 
void SPI::run_test() {
    spi_write_read_blocking(spi_default, this->out_buf, this->in_buf, this->buf_length);

    sleep_ms(10*1000);
}

// Add interrupts
void SPI::run(SPI self) {
    // this->do_something();
}

void SPI::set_interrupt(uint8_t PIN, uint32_t FLAG) {
    gpio_set_irq_enabled(PIN, FLAG, true);
    // irq_set_exclusive_handler(IO_IRQ_BANK0, run); 
}

static void handler() {

}