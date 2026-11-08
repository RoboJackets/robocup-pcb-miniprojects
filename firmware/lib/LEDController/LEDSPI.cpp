#include "LEDSPI.hpp"

LEDSPI::LEDSPI(std::array<uint8_t, PACKET_LENGTH>& packet, std::mutex& p_mutex): 
    packet_(packet), p_mutex_(p_mutex) {}

Error LEDSPI::init() {

    // initializes spi and sets to 1 MHz
    spi_init(SPI_MASTER, 1000*1000);
    spi_init(SPI_SLAVE, 1000*1000);
    spi_set_slave(SPI_SLAVE, true);
    
    // sets SPI mode 3: clock idle is high, data is shifted on falling/leading edge, and data is sampled on rising/trailing edge
    spi_set_format(SPI_MASTER, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_set_format(SPI_SLAVE, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    // sets other pins to SPI
    gpio_set_function(PIN_MISO_MASTER, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_MASTER,   GPIO_FUNC_SIO); // SIO to manually control chip select
    gpio_set_function(PIN_SCK_MASTER,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI_MASTER, GPIO_FUNC_SPI);

    gpio_set_dir(PIN_CS_MASTER, GPIO_OUT);
    gpio_put(PIN_CS_MASTER, 1); // Chip select is active low

    gpio_set_function(PIN_MISO_SLAVE, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_SLAVE,   GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK_SLAVE,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI_SLAVE, GPIO_FUNC_SPI);

    /* <TODO> DMA initialization
    if (dma_claim_unused_channel(false) == -1) {
        return NO_DMA_CHANNEL;
    } */

    return NO_ERROR;
}

Error LEDSPI::read_blocking() {
    // reads from previous device in chain
    gpio_put(PIN_CS_MASTER, 0);
    spi_read_blocking(SPI_MASTER, 0, buffer_.data(), PACKET_LENGTH);
    gpio_put(PIN_CS_MASTER, 1);

    // makes sure previous device bits in checksum are high
    // <TODO> unsure whether debug board sets bit 0, should be 0b00111110 if not
    if ((buffer_.back() & 0b00111111) != 0b00111111) {
        return BAD_CHECKSUM;
    }

    // updates checksum and writes the buffer to the main data store
    buffer_.back() |= (1u << 6);
    std::lock_guard<std::mutex> guard(p_mutex_);
    packet_ = buffer_;

    return NO_ERROR;
}

Error LEDSPI::write_blocking() {
    // writes the main data store to MISO
    std::lock_guard<std::mutex> guard(p_mutex_);
    spi_write_blocking(SPI_SLAVE, packet_.data(), PACKET_LENGTH);

    return NO_ERROR;
}

Error LEDSPI::read_write_blocking() {
    Error error = read_blocking();
    if (error != NO_ERROR) {
        return error;
    } 

    return write_blocking();
}


// <TODO> nonblocking (DMA) functions
Error LEDSPI::read_nonblocking() {}
Error LEDSPI::write_nonblocking() {}
Error LEDSPI::read_write_nonblocking() {}
