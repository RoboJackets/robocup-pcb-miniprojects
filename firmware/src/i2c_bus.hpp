/**
 * @file i2c_bus.hpp
 * @brief I2C bus manager: lets 6 devices safely share the RP2040's 2 I2C controllers.
 *
 * Why this exists
 * ---------------
 * Every I2C device on the board has its own SDA/SCL pair, but the RP2040 has only
 * two controllers, i2c0 and i2c1:
 *
 *   i2c0 <- TCA9534 (4/5), IMU (16/17), Hall (20/21), LED driver (24/25)
 *   i2c1 <- Temperature (18/19), Color (22/23)
 *
 * If one peripheral input is mapped to several pins, the RP2040 ORs their signals
 * together (RP2040 datasheet 2.19.2). An idle I2C line is high, so while any other
 * pair stays attached the controller always sees SDA = 1 and the bus breaks.
 *
 * So each controller is attached to only one pin pair at a time, and when switching,
 * the old pair is detached before the new pair is attached.
 *
 * Multicore safety
 * ----------------
 * Each controller has its own mutex (Pico SDK mutexes work across both cores).
 * Only code holding the lock may switch pins and transfer data, so the two cores
 * can never use the same controller at the same time. i2c0 and i2c1 have separate
 * locks, so the two controllers can work in parallel.
 *
 * Usage
 * -----
 * @code
 *   i2c_bus::init();  // once at startup
 *
 *   {
 *       i2c_bus::BusLock lock(board::Device::GpioExpander);  // lock + route pins
 *       expander.readInputs(value);
 *   }                                                        // unlocked here
 * @endcode
 */
#pragma once

#include <cstdint>

#include "board_config.hpp"
#include "hardware/i2c.h"

namespace i2c_bus {

/// Call once at startup. Inits both controllers and detaches every device's pins.
void init();

/// Locks the device's controller and routes its pins. Blocks until the lock is free.
/// Use BusLock instead of calling this directly.
i2c_inst_t *acquire(board::Device device);

/// Same as acquire() but gives up after timeout_ms and returns nullptr.
i2c_inst_t *tryAcquire(board::Device device, uint32_t timeout_ms);

/// Unlocks. Pins stay routed so using the same device again is free.
void release(board::Device device);

/// Which device a controller is currently routed to (Device::Count if none yet).
board::Device routedDevice(uint8_t controller);

/// Unsticks a bus where a device is holding SDA low (e.g. MCU reset mid-transfer).
/// Clocks SCL up to 9 times then sends a STOP. Call while holding the lock.
/// Returns true if SDA is high afterwards.
bool recover(board::Device device);

/// True if the device ACKs. Takes the lock itself.
bool probe(board::Device device);

/// Locks + routes on construction, unlocks when it goes out of scope.
class BusLock {
public:
    explicit BusLock(board::Device device) : device_(device), i2c_(acquire(device)) {}
    ~BusLock() { release(device_); }

    BusLock(const BusLock &) = delete;
    BusLock &operator=(const BusLock &) = delete;

    i2c_inst_t *i2c() const { return i2c_; }

private:
    board::Device device_;
    i2c_inst_t *i2c_;
};

}  // namespace i2c_bus
