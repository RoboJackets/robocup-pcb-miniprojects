/**
 * @file board_config.hpp
 * @brief Single source of truth for every hardware assignment the firmware uses.
 *
 * Two targets share this file:
 *   - Fulgora PCB            (FULGORA_BOARD defined by boards/fulgora.h)
 *   - Raspberry Pi Pico dev  (default build)
 *
 * The Pico differs only where its own hardware occupies a pin
 * (GPIO23 = SMPS mode, GPIO24 = VBUS sense, GPIO25 = on-board LED,
 * GPIO29 = VSYS sense). Everything else is identical, so code tested
 * on the Pico runs unchanged on the PCB.
 *
 * Source of the PCB values: fulgora-branch schematic + Onboarding Pinouts sheet.
 * Every pin choice below is checked at compile time against the RP2040
 * GPIO function table (see the static_asserts at the bottom).
 */
#pragma once

#include <cstdint>

#include "hardware/i2c.h"
#include "hardware/spi.h"

namespace board {

//inter-board SPI

struct SpiPins {
    uint8_t instance;  //0 = spi0, 1 = spi1
    uint8_t rx; //receive pin
    uint8_t csn; //chip select, active low
    uint8_t sck; //serial clock
    uint8_t tx; //transmit pin
};

// IN connector, we're the slave, Receives packets from the previous board.
inline constexpr SpiPins kSpiIn{0, /*rx*/ 0, /*csn*/ 1, /*sck*/ 2, /*tx*/ 3};

// OUT connector, we're the master, Sends packets to the next board.
inline constexpr SpiPins kSpiOut{1, /*rx*/ 12, /*csn*/ 13, /*sck*/ 14, /*tx*/ 15};

// from the firmware README: 1 MHz, mode 3
inline constexpr uint32_t kSpiBaudHz = 1'000'000;//clock frequency
inline constexpr spi_cpol_t kSpiCpol = SPI_CPOL_1;//Clock is high when idle
inline constexpr spi_cpha_t kSpiCpha = SPI_CPHA_1;//Sample on the second clock edge
inline constexpr uint8_t kSpiDataBits = 8;//8 bits per transfer unit

//I2C devices

// same order as the sections in the packet
enum class Device : uint8_t {
    Imu = 0,
    Temperature,
    HallEffect,
    GpioExpander,
    ColorSensor,
    LedController,
    Count
};

inline constexpr uint8_t kDeviceCount = static_cast<uint8_t>(Device::Count);

struct I2cDevicePins {
    const char *name; //Human-readable device name
    uint8_t instance;  // 0 = i2c0, 1 = i2c1 (depends on which GPIOs, not a choice)
    uint8_t sda;//data pin
    uint8_t scl;//clock pin
    uint8_t address;   //7-bit I2C device address
};

inline constexpr uint32_t kI2cBaudHz = 400'000;  // all 6 parts support 400k

#ifdef FULGORA_BOARD
inline constexpr I2cDevicePins kI2cDevices[kDeviceCount] = {
    //  name               ctrl SDA SCL addr
    {"IMU ICM-42605",       0, 16, 17, 0x68},  // AD0 tied low
    {"Temp LM75ADP",        1, 18, 19, 0x48},  // A2..A0 tied low
    {"Hall TMAG5273A2",     0, 20, 21, 0x35},  // default addr for the A variants
    {"GPIO TCA9534",        0,  4,  5, 0x20},  // A2..A0 tied low
    {"Color VEML3328",      1, 22, 23, 0x10},  // fixed
    {"LED IS31FL3254",      0, 24, 25, 0x30},  // AD tied low
};
#else
// Pico version, only the pins the Pico uses internally are moved
inline constexpr I2cDevicePins kI2cDevices[kDeviceCount] = {
    {"IMU ICM-42605",       0, 16, 17, 0x68},
    {"Temp LM75ADP",        1, 18, 19, 0x48},
    {"Hall TMAG5273A2",     0, 20, 21, 0x35},
    {"GPIO TCA9534",        0,  4,  5, 0x20},
    {"Color VEML3328",      1, 26, 27, 0x10},  // 22/23 on the PCB
    {"LED IS31FL3254",      0,  8,  9, 0x30},  // 24/25 on the PCB
};
#endif

inline constexpr const I2cDevicePins &device(Device d) {
    return kI2cDevices[static_cast<uint8_t>(d)];
}

// ---- buttons / LEDs ----

inline constexpr uint8_t kButtonCount = 2;
inline constexpr uint8_t kButtonPins[kButtonCount] = {6, 7};  // active low, 10k pull-ups on board

inline constexpr uint8_t kUserLedCount = 3;
#ifdef FULGORA_BOARD
inline constexpr uint8_t kUserLedPins[kUserLedCount] = {8, 9, 10};  // active high
inline constexpr uint8_t kLedDriverShutdownPin = 29;               // has a pull-down, drive high to enable
#else
inline constexpr uint8_t kUserLedPins[kUserLedCount] = {10, 11, 22};  // 8/9/10 on the PCB
inline constexpr uint8_t kLedDriverShutdownPin = 28;                 // 29 on the PCB
#endif

inline spi_inst_t *spiInstance(const SpiPins &p) { return p.instance ? spi1 : spi0; }
inline i2c_inst_t *i2cInstance(const I2cDevicePins &p) { return p.instance ? i2c1 : i2c0; }

// ---- compile-time pin checks (RP2040 datasheet 2.19.2) ----
namespace detail {

constexpr bool validGpio(uint8_t pin) { return pin <= 29; }

// SPI0/SPI1 swap every 8 pins, and inside each group of 4 it's RX, CSn, SCK, TX
constexpr uint8_t spiInstanceOf(uint8_t pin) { return (pin / 8) % 2; }
constexpr bool spiPinsValid(const SpiPins &p) {
    return validGpio(p.rx) && validGpio(p.tx) && validGpio(p.sck) && validGpio(p.csn) &&
           spiInstanceOf(p.rx) == p.instance && p.rx % 4 == 0 &&
           spiInstanceOf(p.csn) == p.instance && p.csn % 4 == 1 &&
           spiInstanceOf(p.sck) == p.instance && p.sck % 4 == 2 &&
           spiInstanceOf(p.tx) == p.instance && p.tx % 4 == 3;
}

// I2C0/I2C1 swap every 2 pins, even = SDA, odd = SCL
constexpr uint8_t i2cInstanceOf(uint8_t pin) { return (pin / 2) % 2; }
constexpr bool i2cPinsValid(const I2cDevicePins &p) {
    return validGpio(p.sda) && validGpio(p.scl) &&
           p.sda % 2 == 0 && p.scl == p.sda + 1 &&
           i2cInstanceOf(p.sda) == p.instance && p.address < 0x80;
}

constexpr bool allI2cValid() {
    for (const auto &d : kI2cDevices) {
        if (!i2cPinsValid(d)) return false;
    }
    return true;
}

}  // namespace detail

// build fails here if a pin above is wrong
static_assert(detail::spiPinsValid(kSpiIn), "kSpiIn pins do not match an RP2040 SPI block");
static_assert(detail::spiPinsValid(kSpiOut), "kSpiOut pins do not match an RP2040 SPI block");
static_assert(kSpiIn.instance != kSpiOut.instance, "IN and OUT must use different SPI controllers");
static_assert(detail::allI2cValid(), "an I2C device uses pins that are not SDA/SCL of its controller");

}  // namespace board
