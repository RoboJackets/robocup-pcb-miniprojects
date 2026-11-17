/**
 * @file fulgora.h
 * @brief Pico SDK board definition for the Fulgora sensor-suite PCB.
 *
 * Selected with -DFULGORA_DEV_PICO=OFF. Hardware facts come from the
 * fulgora-branch schematic (RP2040 + W25Q16JV 2 MB flash + 12 MHz ABM8-272).
 *
 * Only SDK-level defaults live here. Application pin assignments
 * (SPI, I2C buses, buttons) belong in the application's board config.
 */
#ifndef _BOARDS_FULGORA_H
#define _BOARDS_FULGORA_H

pico_board_cmake_set(PICO_PLATFORM, rp2040)

#define FULGORA_BOARD

// no default UART on purpose, GPIO0/1 are SPI0

// LED1 on GPIO8
#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN 8
#endif

// flash is a W25Q16JV (2 MB), same family as the Pico's
#define PICO_BOOT_STAGE2_CHOOSE_W25Q080 1

#ifndef PICO_FLASH_SPI_CLKDIV
#define PICO_FLASH_SPI_CLKDIV 2
#endif

pico_board_cmake_set_default(PICO_FLASH_SIZE_BYTES, (2 * 1024 * 1024))
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (2 * 1024 * 1024)
#endif

// 12 MHz crystal, give it extra time to start since it's a custom board
#ifndef PICO_XOSC_STARTUP_DELAY_MULTIPLIER
#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64
#endif

#ifndef PICO_RP2040_B0_SUPPORTED
#define PICO_RP2040_B0_SUPPORTED 0
#endif

#endif // _BOARDS_FULGORA_H
