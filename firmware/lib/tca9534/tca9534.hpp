/**
 * @file tca9534.hpp
 * @brief Driver for the TI TCA9534 8-bit I2C GPIO expander (Pico SDK).
 *
 * About the chip
 * --------------
 * The TCA9534 adds 8 GPIO pins (P0-P7) over I2C; each pin can be an input or an output.
 *
 * The chip has only four 8-bit registers; bit n of each register belongs to pin Pn:
 *
 *   | Addr | Register           | Reset | Meaning                         |
 *   |------|--------------------|-------|---------------------------------|
 *   | 0x00 | Input Port         | pins  | read-only: current pin levels   |
 *   | 0x01 | Output Port        | 0xFF  | level driven on output pins     |
 *   | 0x02 | Polarity Inversion | 0x00  | 1 = invert that input bit       |
 *   | 0x03 | Configuration      | 0xFF  | 1 = input, 0 = output           |
 *
 * I2C address
 * -----------
 * 7-bit address = 0b0100 A2 A1 A0, range 0x20-0x27 (set by how pins A2..A0 are wired).
 * Note: the TCA9534A is a different part whose range is 0x38-0x3F.
 *
 * Bus protocol
 * ------------
 * Write: [addr+W] [register] [data]
 * Read:  [addr+W] [register] [repeated start] [addr+R] [data]
 * The chip has no auto-increment, so every access touches exactly one register.
 *
 * Usage example
 * -------------
 * @code
 *   i2c_init(i2c0, 400'000);
 *   gpio_set_function(4, GPIO_FUNC_I2C);  // SDA
 *   gpio_set_function(5, GPIO_FUNC_I2C);  // SCL
 *
 *   tca9534::Tca9534 expander(i2c0, 0x20);
 *   if (expander.init() == tca9534::Status::Ok) {
 *       uint8_t switches = 0;
 *       expander.readInputs(switches);  // all 8 pins at once
 *
 *       expander.pinMode(7, tca9534::Direction::Output);
 *       expander.digitalWrite(7, true);
 *   }
 * @endcode
 *
 * Thread safety
 * -------------
 * The driver does not lock anything. If both cores can use the same I2C controller,
 * the caller must protect the calls with a mutex.
 */
#pragma once

#include <cstdint>

#include "hardware/i2c.h"

namespace tca9534 {

enum class Reg : uint8_t {
    Input = 0x00,     // read-only
    Output = 0x01,
    Polarity = 0x02,  // 1 = invert that input bit
    Config = 0x03,    // 1 = input, 0 = output
};

// values match the Config register bits
enum class Direction : uint8_t {
    Output = 0,
    Input = 1,
};

enum class Status : uint8_t {
    Ok = 0,
    NoAck,           // nothing answered at that address (wiring / wrong addr)
    Timeout,         // bus stuck
    InvalidPin,      // pin not 0-7
    InvalidAddress,  // not in 0x20-0x27
    NotInitialized,  // need to call init() first
};

/// Status as a string, for printf.
const char *toString(Status status);

class Tca9534 {
public:
    static constexpr uint8_t kPinCount = 8;
    static constexpr uint8_t kBaseAddress = 0x20;  // A2..A0 all low
    static constexpr uint8_t kMaxAddress = 0x27;   // A2..A0 all high
    static constexpr uint32_t kDefaultTimeoutUs = 1000;

    // power-on values from the datasheet
    static constexpr uint8_t kResetOutput = 0xFF;
    static constexpr uint8_t kResetPolarity = 0x00;
    static constexpr uint8_t kResetConfig = 0xFF;

    /// Address from how A2/A1/A0 are wired (true = tied high).
    static constexpr uint8_t addressFromPins(bool a2, bool a1, bool a0) {
        return static_cast<uint8_t>(kBaseAddress | (a2 << 2) | (a1 << 1) | (a0 << 0));
    }

    /// Just stores the args, doesn't touch the bus. Set up the I2C controller and pins yourself first.
    explicit Tca9534(i2c_inst_t *i2c, uint8_t address = kBaseAddress,
                     uint32_t timeout_us = kDefaultTimeoutUs);

    // ---- setup ----

    /// Checks the chip is there and loads its current Output/Polarity/Config into the cache.
    /// Doesn't change anything on the chip. Needed before the single-pin functions.
    Status init();

    /// Puts the 3 writable registers back to power-on values (all inputs, no inversion).
    Status resetToDefaults();

    /// True if the chip ACKs. Doesn't need init().
    bool isPresent();

    // ---- whole port (bit n = pin Pn) ----

    /// Reads all 8 pins (after polarity inversion). Also saved for lastInputs().
    Status readInputs(uint8_t &inputs);

    /// 1 = input, 0 = output.
    Status setDirection(uint8_t config);
    Status getDirection(uint8_t &config);

    /// Only affects pins set as outputs.
    Status writeOutputs(uint8_t outputs);
    /// Returns what was written, not the actual pin level (use readInputs for that).
    Status getOutputs(uint8_t &outputs);

    /// 1 = invert that pin when reading. e.g. 0xFF makes a grounded switch read as 1.
    Status setPolarity(uint8_t polarity);
    Status getPolarity(uint8_t &polarity);

    // ---- single pin (need init() first) ----

    Status pinMode(uint8_t pin, Direction direction);
    Status digitalWrite(uint8_t pin, bool high);
    Status togglePin(uint8_t pin);
    /// Doesn't need init(), reads the whole port and picks out the bit.
    Status digitalRead(uint8_t pin, bool &high);
    Status setPinPolarity(uint8_t pin, bool inverted);

    // ---- change detection ----

    /// Reads the inputs and sets `changed` to the bits that flipped since the last read.
    /// We use this instead of the INT pin (not allowed to use interrupt lines).
    /// First call always reports no change.
    Status readChanges(uint8_t &inputs, uint8_t &changed);

    // ---- raw register access ----

    Status readRegister(Reg reg, uint8_t &value);
    /// Keeps the cache in sync. Writing Input does nothing on the chip.
    Status writeRegister(Reg reg, uint8_t value);

    uint8_t address() const { return address_; }
    bool initialized() const { return initialized_; }
    uint8_t lastInputs() const { return last_inputs_; }  // no bus access
    Status lastError() const { return last_error_; }

private:
    Status checkPin(uint8_t pin);
    Status updateBit(Reg reg, uint8_t &cache, uint8_t pin, bool set);
    Status fail(Status status);

    i2c_inst_t *i2c_;
    uint8_t address_;
    uint32_t timeout_us_;

    // local copy of the chip's registers so single-pin writes don't need a read first
    uint8_t output_ = kResetOutput;
    uint8_t polarity_ = kResetPolarity;
    uint8_t config_ = kResetConfig;

    uint8_t last_inputs_ = 0;
    bool have_inputs_ = false;
    bool initialized_ = false;
    Status last_error_ = Status::Ok;
};

}  // namespace tca9534
