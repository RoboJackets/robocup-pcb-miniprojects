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
    // TODO: see the I2C Address Map in the HRM for pins + controller per device
    return i2c0;
}

// Called by the repeating timer every IMU_SAMPLE_PERIOD_MS.
// Keep it short: just set a flag; do the I2C read in the main loop.
static volatile bool sample_due = false;
static bool sample_timer_cb(repeating_timer_t *t) {
    sample_due = true;
    return true;   // keep repeating
}

// ---- public ----

void core1_main(void) {
    // TODO: i2c_init(i2c0 / i2c1, I2C_BAUD_HZ)
    // TODO: imu_init(i2c_bus_select(DEV_IMU), IMU_DEFAULT_ADDR)
    ui_init();

    // The SDK's default alarm pool fires its interrupt on core 0, so core 1
    // creates its own pool here; its interrupt then runs on core 1 and can
    // wake core 1 from __wfi().
    static repeating_timer_t timer;
    alarm_pool_t *pool = alarm_pool_create_with_unused_hardware_alarm(4);
    alarm_pool_add_repeating_timer_ms(pool, -IMU_SAMPLE_PERIOD_MS, sample_timer_cb, NULL, &timer);

    uint8_t sample[IMU_DATA_LEN];
    while (true) {
        if (sample_due) {
            sample_due = false;
            if (imu_read(sample)) {          // slow I2C read, outside the lock
                shared_set_imu(sample);       // short copy, inside the lock
            }
        }
        ui_update();
        __wfi();   // sleep until the next interrupt (timer or button)
    }
}
