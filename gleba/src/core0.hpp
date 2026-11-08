#pragma once
// Core 0: SPI daisy chain.
// Receive a packet on SPI0 (slave) -> put the latest IMU data in bytes 0-11
// -> set our bit in byte 55 -> send on SPI1 (master).
// Owner: Erin

#include <stdint.h>

#define PACKET_LEN        56
#define PACKET_IMU_OFFSET 0     // IMU data goes in bytes 0-11
#define PACKET_ID_BYTE    55    // board-ID bitmap
#define BOARD_ID_BIT      (1u << 1)   // IMU board = bit 1

// Set up SPI0 as slave and SPI1 as master (1 MHz, mode 3) and their pins.
void core0_init(void);

// Main loop of core 0. Never returns.
void core0_main(void);