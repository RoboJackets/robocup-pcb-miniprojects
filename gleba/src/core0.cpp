#include "core0.hpp"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pins.hpp"
#include "shared.hpp"
#include <string.h> // memcpy


// ---- internal helpers (only used in this file) ----

// Wait for one full packet from the previous board.
static bool spi_in_receive(uint8_t packet[PACKET_LEN]) {
    while (!gpio_get(PIN_SPI_IN_CSN)) {
        tight_loop_contents();
    }

    while (spi_is_readable(spi0)) {
        (void) spi_get_hw(spi0)->dr;
    }

    spi_read_blocking(spi0, 0, packet, PACKET_LEN);

    if (packet[PACKET_ID_BYTE] == 0) {
        return false;
    }
    return true;
}

// Copy the latest IMU sample into bytes 0-11 and set our bit in byte 55.
static void packet_fill(uint8_t packet[PACKET_LEN]) {
    // TODO: shared_get_imu(...) -> copy to packet + PACKET_IMU_OFFSET
    //       packet[PACKET_ID_BYTE] |= BOARD_ID_BIT;
    uint8_t imu[IMU_DATA_LEN]; // imu reading space
    shared_get_imu(imu); // most recent reading
    memcpy(packet + PACKET_IMU_OFFSET, imu, IMU_DATA_LEN); // reading into bytes 0-11
    packet[PACKET_ID_BYTE] |= BOARD_ID_BIT; // IMU board added
}

// Send the packet to the next board.
static void spi_out_send(const uint8_t packet[PACKET_LEN]) {
    gpio_put(PIN_SPI_OUT_CSN, 0); // CS low
    spi_write_blocking(spi1, packet, PACKET_LEN); // send 56 bytes
    gpio_put(PIN_SPI_OUT_CSN, 1); // CS high
}

// ---- public ----

void core0_init(void) {
    // TODO: spi_init / spi_set_format / spi_set_slave / gpio_set_function for both ports

    // SP0 (Slave)
    spi_init(spi0, SPI_BAUD_HZ); // turn on spi0 at 1 MHz
    spi_set_format(spi0, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST); // Settings, Mode 3
    spi_set_slave(spi0, true); // slave

    gpio_set_function(PIN_SPI_IN_SCK, GPIO_FUNC_SPI); // clock
    gpio_set_function(PIN_SPI_IN_MOSI, GPIO_FUNC_SPI); // data come in
    gpio_set_function(PIN_SPI_IN_MISO, GPIO_FUNC_SPI); // data come out
    gpio_set_function(PIN_SPI_IN_CSN, GPIO_FUNC_SPI); // chip select

    // SP1 (Master)
    spi_init(spi1, SPI_BAUD_HZ); // turn on spi1 at 1 MHz
    spi_set_format(spi1, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST); // Settings, Mode 3
    spi_set_slave(spi1, false); // master

    gpio_set_function(PIN_SPI_OUT_SCK, GPIO_FUNC_SPI); // clock
    gpio_set_function(PIN_SPI_OUT_MOSI, GPIO_FUNC_SPI); // data come out
    gpio_set_function(PIN_SPI_OUT_MISO, GPIO_FUNC_SPI); // data come in

    gpio_init(PIN_SPI_OUT_CSN);
    gpio_set_dir(PIN_SPI_OUT_CSN, GPIO_OUT);
    gpio_put(PIN_SPI_OUT_CSN, 1);
}

void core0_main(void) {
    uint8_t packet[PACKET_LEN];
    while (true) {
        if (!spi_in_receive(packet)) {
            continue;
        }
        packet_fill(packet);
        spi_out_send(packet);
    }
}
