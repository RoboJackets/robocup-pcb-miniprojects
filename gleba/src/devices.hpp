#pragma once
// NEW: one table that describes the six I2C devices (controller, pins,
// address, where the data goes in the SPI packet, board-ID bit).
// Owner: Yueqi

#include <stdint.h>
#include "hardware/i2c.h"

#define MAX_SECTION_LEN 24   // longest packet section of one device (LED driver)

// The six I2C devices on the board (selected with the buttons)
enum Device : uint8_t {
    DEV_IMU = 0,
    DEV_TEMP,
    DEV_HALL,
    DEV_GPIO,    // GPIO expander + DIP switch
    DEV_COLOR,
    DEV_LED,     // LED driver
    DEV_COUNT    // NEW: number of devices (6). Keep it last.
};

// NEW: everything we need to know about one device.
struct DeviceInfo {
    i2c_inst_t *i2c;         // i2c0 or i2c1
    uint8_t     sda_pin;
    uint8_t     scl_pin;
    uint8_t     addr;        // 7-bit I2C address
    uint8_t     pkt_offset;  // first packet byte of this device's section
    uint8_t     pkt_len;     // number of bytes in that section
    uint8_t     id_bit;      // mask for packet[PACKET_ID_BYTE]
};

// NEW: look up the row of one device.
const DeviceInfo *device_info(Device dev);
