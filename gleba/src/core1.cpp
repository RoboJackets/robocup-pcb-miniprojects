#include "core1.hpp"

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/sync.h"   // __wfi()
#include "driver.hpp"
#include "pins.hpp"
#include "shared.hpp"
#include "ui.hpp"

// ---- internal helpers ----

// Route the right I2C controller (i2c0 or i2c1) to the pin pair of `dev`
// and switch the other pairs on that controller back to plain GPIO.
// Returns the controller to use with that device.
static i2c_inst_t *i2c_bus_select(Device dev) {
    
}

// Called by the repeating timer every IMU_SAMPLE_PERIOD_MS.
// Keep it short: just set a flag; do the I2C read in the main loop.
static volatile bool sample_due = false;

// ---- public ----

void core1_main(void) {
    i2c_init(i2c0 / i2c1, I2C_BAUD_HZ);
    imu_init(i2c_bus_select(DEV_IMU), IMU_DEFAULT_ADDR);
    ui_init();

    uint8_t sample[IMU_DATA_LEN];
    while (true) {
        // To be implemented
    }
}
