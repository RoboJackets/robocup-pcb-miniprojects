/**
 * @file tca9534.cpp
 * @brief TCA9534 driver implementation.
 */
#include "tca9534.hpp"

namespace tca9534 {

namespace {

// SDK returns the byte count on success, negative on error
Status fromSdkResult(int result, int expected) {
    if (result == expected) return Status::Ok;
    if (result == PICO_ERROR_TIMEOUT) return Status::Timeout;
    return Status::NoAck;  // PICO_ERROR_GENERIC = address got NACKed
}

constexpr uint8_t bitOf(uint8_t pin) { return static_cast<uint8_t>(1u << pin); }

}  // namespace

const char *toString(Status status) {
    switch (status) {
        case Status::Ok: return "Ok";
        case Status::NoAck: return "NoAck";
        case Status::Timeout: return "Timeout";
        case Status::InvalidPin: return "InvalidPin";
        case Status::InvalidAddress: return "InvalidAddress";
        case Status::NotInitialized: return "NotInitialized";
    }
    return "Unknown";
}

Tca9534::Tca9534(i2c_inst_t *i2c, uint8_t address, uint32_t timeout_us)
    : i2c_(i2c), address_(address), timeout_us_(timeout_us) {}

// ---- setup ----

Status Tca9534::init() {
    initialized_ = false;

    if (address_ < kBaseAddress || address_ > kMaxAddress) {
        return fail(Status::InvalidAddress);
    }

    // read the real values instead of assuming defaults, the MCU might have
    // reset while the chip stayed powered
    Status s = readRegister(Reg::Output, output_);
    if (s != Status::Ok) return s;
    s = readRegister(Reg::Polarity, polarity_);
    if (s != Status::Ok) return s;
    s = readRegister(Reg::Config, config_);
    if (s != Status::Ok) return s;

    have_inputs_ = false;
    initialized_ = true;
    last_error_ = Status::Ok;
    return Status::Ok;
}

Status Tca9534::resetToDefaults() {
    // config first so nothing drives a weird value in between
    Status s = writeRegister(Reg::Config, kResetConfig);
    if (s != Status::Ok) return s;
    s = writeRegister(Reg::Polarity, kResetPolarity);
    if (s != Status::Ok) return s;
    return writeRegister(Reg::Output, kResetOutput);
}

bool Tca9534::isPresent() {
    uint8_t value = 0;
    return readRegister(Reg::Input, value) == Status::Ok;
}

//whole port

Status Tca9534::readInputs(uint8_t &inputs) {
    const Status s = readRegister(Reg::Input, inputs);
    if (s == Status::Ok) {
        last_inputs_ = inputs;
        have_inputs_ = true;
    }
    return s;
}

Status Tca9534::setDirection(uint8_t config) { return writeRegister(Reg::Config, config); }

Status Tca9534::getDirection(uint8_t &config) { return readRegister(Reg::Config, config); }

Status Tca9534::writeOutputs(uint8_t outputs) { return writeRegister(Reg::Output, outputs); }

Status Tca9534::getOutputs(uint8_t &outputs) { return readRegister(Reg::Output, outputs); }

Status Tca9534::setPolarity(uint8_t polarity) { return writeRegister(Reg::Polarity, polarity); }

Status Tca9534::getPolarity(uint8_t &polarity) { return readRegister(Reg::Polarity, polarity); }

//single pin
Status Tca9534::pinMode(uint8_t pin, Direction direction) {
    const Status s = checkPin(pin);
    if (s != Status::Ok) return s;
    return updateBit(Reg::Config, config_, pin, direction == Direction::Input);
}

Status Tca9534::digitalWrite(uint8_t pin, bool high) {
    const Status s = checkPin(pin);
    if (s != Status::Ok) return s;
    return updateBit(Reg::Output, output_, pin, high);
}

Status Tca9534::togglePin(uint8_t pin) {
    const Status s = checkPin(pin);
    if (s != Status::Ok) return s;
    const bool currently_high = output_ & bitOf(pin);
    return updateBit(Reg::Output, output_, pin, !currently_high);
}

Status Tca9534::digitalRead(uint8_t pin, bool &high) {
    // no cache involved, so init() isn't required here
    if (pin >= kPinCount) return fail(Status::InvalidPin);

    uint8_t inputs = 0;
    const Status s = readInputs(inputs);
    if (s == Status::Ok) high = inputs & bitOf(pin);
    return s;
}

Status Tca9534::setPinPolarity(uint8_t pin, bool inverted) {
    const Status s = checkPin(pin);
    if (s != Status::Ok) return s;
    return updateBit(Reg::Polarity, polarity_, pin, inverted);
}

//change detection

Status Tca9534::readChanges(uint8_t &inputs, uint8_t &changed) {
    const bool had_previous = have_inputs_;
    const uint8_t previous = last_inputs_;

    const Status s = readInputs(inputs);
    if (s != Status::Ok) return s;

    // XOR leaves a 1 wherever the bit flipped
    changed = had_previous ? static_cast<uint8_t>(inputs ^ previous) : 0;
    return Status::Ok;
}

//raw access
Status Tca9534::readRegister(Reg reg, uint8_t &value) {
    const uint8_t command = static_cast<uint8_t>(reg);

    // send the register number, nostop=true so we do a repeated start instead of STOP
    int r = i2c_write_timeout_us(i2c_, address_, &command, 1, true, timeout_us_);
    Status s = fromSdkResult(r, 1);
    if (s != Status::Ok) return fail(s);

    r = i2c_read_timeout_us(i2c_, address_, &value, 1, false, timeout_us_);
    s = fromSdkResult(r, 1);
    if (s != Status::Ok) return fail(s);

    return Status::Ok;
}

Status Tca9534::writeRegister(Reg reg, uint8_t value) {
    const uint8_t frame[2] = {static_cast<uint8_t>(reg), value};

    const int r = i2c_write_timeout_us(i2c_, address_, frame, 2, false, timeout_us_);
    const Status s = fromSdkResult(r, 2);
    if (s != Status::Ok) return fail(s);

    // only update the cache once the write actually went through
    switch (reg) {
        case Reg::Output: output_ = value; break;
        case Reg::Polarity: polarity_ = value; break;
        case Reg::Config: config_ = value; break;
        case Reg::Input: break;
    }
    return Status::Ok;
}

//helpers

Status Tca9534::checkPin(uint8_t pin) {
    if (pin >= kPinCount) return fail(Status::InvalidPin);
    if (!initialized_) return fail(Status::NotInitialized);
    return Status::Ok;
}

// read-modify-write using the cache, so it's one I2C write instead of read + write
Status Tca9534::updateBit(Reg reg, uint8_t &cache, uint8_t pin, bool set) {
    const uint8_t updated = set ? static_cast<uint8_t>(cache | bitOf(pin))
                                : static_cast<uint8_t>(cache & ~bitOf(pin));

    if (updated == cache) return Status::Ok;  // nothing changed, skip the bus

    return writeRegister(reg, updated);
}

Status Tca9534::fail(Status status) {
    last_error_ = status;
    return status;
}

}  //namespace tca9534
