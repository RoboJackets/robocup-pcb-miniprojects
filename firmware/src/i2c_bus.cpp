/**
 * @file i2c_bus.cpp
 * @brief I2C bus manager implementation.
 */
#include "i2c_bus.hpp"

#include "hardware/gpio.h"
#include "pico/mutex.h"
#include "pico/time.h"

namespace i2c_bus {

namespace {

constexpr uint8_t kControllerCount = 2;
constexpr uint8_t kNoDevice = board::kDeviceCount;

mutex_t g_lock[kControllerCount];

// which device each controller is hooked up to right now (only touched while locked)
uint8_t g_routed[kControllerCount] = {kNoDevice, kNoDevice};

bool g_initialized = false;

const board::I2cDevicePins &pinsOf(board::Device device) { return board::device(device); }

// FUNC_NULL = pin stops driving, the pull-ups keep the line high
void detachPins(const board::I2cDevicePins &p) {
    gpio_set_function(p.sda, GPIO_FUNC_NULL);
    gpio_set_function(p.scl, GPIO_FUNC_NULL);
}

void attachPins(const board::I2cDevicePins &p) {
    gpio_set_function(p.sda, GPIO_FUNC_I2C);
    gpio_set_function(p.scl, GPIO_FUNC_I2C);
}

// caller must already hold this controller's lock
void route(board::Device device) {
    const uint8_t index = static_cast<uint8_t>(device);
    const uint8_t ctrl = pinsOf(device).instance;

    if (g_routed[ctrl] == index) return;

    // detach the old pair BEFORE attaching the new one. if two pairs are on the
    // same controller at once the RP2040 ORs their inputs and SDA reads stuck high
    if (g_routed[ctrl] != kNoDevice) {
        detachPins(pinsOf(static_cast<board::Device>(g_routed[ctrl])));
    }
    attachPins(pinsOf(device));
    g_routed[ctrl] = index;
}

}  // namespace

void init() {
    if (g_initialized) return;

    for (uint8_t c = 0; c < kControllerCount; ++c) {
        mutex_init(&g_lock[c]);
        i2c_init(c ? i2c1 : i2c0, board::kI2cBaudHz);
        g_routed[c] = kNoDevice;
    }

    for (const auto &p : board::kI2cDevices) {
        // PCB already has 2.2k pull-ups, this is just so a bare Pico doesn't float
        gpio_pull_up(p.sda);
        gpio_pull_up(p.scl);
        detachPins(p);
    }

    g_initialized = true;
}

i2c_inst_t *acquire(board::Device device) {
    const uint8_t ctrl = pinsOf(device).instance;
    mutex_enter_blocking(&g_lock[ctrl]);
    route(device);
    return ctrl ? i2c1 : i2c0;
}

i2c_inst_t *tryAcquire(board::Device device, uint32_t timeout_ms) {
    const uint8_t ctrl = pinsOf(device).instance;
    if (!mutex_enter_timeout_ms(&g_lock[ctrl], timeout_ms)) return nullptr;
    route(device);
    return ctrl ? i2c1 : i2c0;
}

void release(board::Device device) { mutex_exit(&g_lock[pinsOf(device).instance]); }

board::Device routedDevice(uint8_t controller) {
    if (controller >= kControllerCount) return board::Device::Count;
    return static_cast<board::Device>(g_routed[controller]);
}

bool recover(board::Device device) {
    const auto &p = pinsOf(device);
    constexpr uint32_t kHalfPeriodUs = 5;  // ~100 kHz

    // bit-bang with SIO. fake open-drain by flipping direction:
    // output (value 0) = pull low, input = let the pull-up take it high
    gpio_init(p.sda);
    gpio_init(p.scl);
    gpio_put(p.sda, false);
    gpio_put(p.scl, false);
    gpio_set_dir(p.sda, GPIO_IN);
    gpio_set_dir(p.scl, GPIO_IN);
    sleep_us(kHalfPeriodUs);

    // clock until the device lets go of SDA (max 9, one full byte + ack)
    for (int i = 0; i < 9 && !gpio_get(p.sda); ++i) {
        gpio_set_dir(p.scl, GPIO_OUT);
        sleep_us(kHalfPeriodUs);
        gpio_set_dir(p.scl, GPIO_IN);
        sleep_us(kHalfPeriodUs);
    }

    // STOP = SDA low -> high while SCL is high
    gpio_set_dir(p.sda, GPIO_OUT);
    sleep_us(kHalfPeriodUs);
    gpio_set_dir(p.sda, GPIO_IN);
    sleep_us(kHalfPeriodUs);

    const bool sda_high = gpio_get(p.sda);

    // gpio_init cleared the pull-ups, put them back and hand the pins back to I2C
    gpio_pull_up(p.sda);
    gpio_pull_up(p.scl);
    attachPins(p);
    g_routed[p.instance] = static_cast<uint8_t>(device);
    return sda_high;
}

bool probe(board::Device device) {
    BusLock lock(device);
    uint8_t dummy = 0;
    // 1-byte read, if the address ACKs it's there (same trick as the SDK bus_scan example)
    const int r = i2c_read_timeout_us(lock.i2c(), pinsOf(device).address, &dummy, 1, false, 1000);
    return r == 1;
}

}  // namespace i2c_bus
