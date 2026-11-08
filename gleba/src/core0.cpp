#include "core0.hpp"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pins.hpp"
#include "shared.hpp"

// ---- internal helpers (only used in this file) ----

// Wait for one full packet from the previous board. Returns false if the
// packet was incomplete (e.g. CS went high before 56 bytes) and should be dropped.
static bool spi_in_receive(uint8_t packet[PACKET_LEN]) {
    // TODO
    return false;
}

// Copy the latest IMU sample into bytes 0-11 and set our bit in byte 55.
static void packet_fill(uint8_t packet[PACKET_LEN]) {
    // TODO: shared_get_imu(...) -> copy to packet + PACKET_IMU_OFFSET
    //       packet[PACKET_ID_BYTE] |= BOARD_ID_BIT;
}

// Send the packet to the next board.
static void spi_out_send(const uint8_t packet[PACKET_LEN]) {
    // TODO
}

// ---- public ----

void core0_init(void) {
    // TODO: spi_init / spi_set_format / spi_set_slave / gpio_set_function for both ports
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