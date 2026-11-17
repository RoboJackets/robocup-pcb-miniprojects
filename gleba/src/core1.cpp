#include "core1.hpp"

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "driver.hpp"
#include "pins.hpp"
#include "shared.hpp"
#include "ui.hpp"

#if 0
---- internal helpers ----

NEW: device whose pins are connected to an I2C controller right now
(DEV_COUNT = none yet).
static Device active = DEV_COUNT;

Route the right I2C controller (i2c0 or i2c1) to the pin pair of `dev`
and switch the other pairs on that controller back to plain GPIO.
Returns the controller to use with that device.
static i2c_inst_t *i2c_bus_select(Device dev) {
    switch (dev)
    {
    case DEV_IMU:
    case DEV_HALL:
    case DEV_COLOR:
        return i2c0;
    case DEV_TEMP:
    case DEV_GPIO:
    case DEV_LED:
        return i2c1;
    }
}

static void device_init() {
    i2c_init(i2c_bus_select(DEV_IMU), I2C_BAUD_HZ);
}
#endif

static void device_init() {
    i2c_init(i2c0, I2C_BAUD_HZ);
    gpio_set_function(PIN_SDA_IMU, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL_IMU, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA_IMU); // Initialize SDA
    gpio_pull_up(PIN_SCL_IMU); // Initialize SCL
    imu_init(i2c0, IMU_ADDR_DEFAULT);
}

// // NEW: read the packet section of `dev` into `out`. Returns true if `out`
// // holds fresh data.
// static bool read_device(Device dev, uint8_t out[MAX_SECTION_LEN]) {
//     // TODO
//     return false;
// }

static bool read_imu(uint8_t out[IMU_DATA_LEN]) {
    return imu_read_raw(out);
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
    ui_init();
    device_init();
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
            if (read_imu(sample)) {
                shared_set_imu(sample);
            }
        }
        // ui_update();
        __wfi();   // sleep until the next interrupt (timer or button)
    }
}
