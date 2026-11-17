# TCA9534 Driver

Pico SDK driver for the TI TCA9534 8-bit I2C GPIO expander. Depends only on `hardware_i2c`.

## Add to your project

```cmake
add_subdirectory(lib/tca9534)
target_link_libraries(your_target tca9534)
```

## Quick start

```cpp
#include "tca9534.hpp"

// set up the I2C controller and pins first
i2c_init(i2c0, 400'000);
gpio_set_function(4, GPIO_FUNC_I2C);  // SDA
gpio_set_function(5, GPIO_FUNC_I2C);  // SCL

tca9534::Tca9534 expander(i2c0, 0x20);   // A2..A0 = GND -> 0x20
if (expander.init() != tca9534::Status::Ok) {
    // check wiring and address
}

uint8_t inputs = 0;
expander.readInputs(inputs);             // all 8 pins

expander.pinMode(7, tca9534::Direction::Output);
expander.digitalWrite(7, true);          // drive P7 high
```

## API overview

| Function | Purpose | Needs `init()` |
|---|---|---|
| `init()` | check the chip, load its current settings | — |
| `isPresent()` | only check for ACK | no |
| `resetToDefaults()` | restore power-on values | no |
| `readInputs(uint8_t&)` | read all 8 pins | no |
| `setDirection(uint8_t)` / `getDirection(uint8_t&)` | direction, 1 = input | no |
| `writeOutputs(uint8_t)` / `getOutputs(uint8_t&)` | output register | no |
| `setPolarity(uint8_t)` / `getPolarity(uint8_t&)` | input inversion | no |
| `pinMode(pin, Direction)` | one pin's direction | yes |
| `digitalWrite(pin, bool)` / `togglePin(pin)` | one pin's output | yes |
| `digitalRead(pin, bool&)` | read one pin | no |
| `setPinPolarity(pin, bool)` | one pin's inversion | yes |
| `readChanges(inputs&, changed&)` | read inputs and report which pins changed | no |
| `readRegister()` / `writeRegister()` | raw register access | no |
| `lastError()` / `lastInputs()` | last error and last inputs read | — |

Every operation returns a `tca9534::Status`; `tca9534::toString()` turns it into a string for printing.

## Notes

- The address range is 0x20–0x27. The TCA9534**A** is a different part at 0x38–0x3F.
- `getOutputs()` returns the value that was set, not the real pin level; use `readInputs()` for that.
- Single-pin writes do read-modify-write on a local cache, so each one costs a single I2C write, and nothing is sent if the value doesn't change.
- The driver does not lock. When cores or devices share an I2C controller, the caller must use a mutex.
- Without the INT pin, poll `readChanges()` instead of using an interrupt.
