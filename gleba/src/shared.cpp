#include "shared.hpp"

#include <string.h>
#include "pico/sync.h"

// static: only this file can see these, so every access goes through the
// functions below (which always take the lock).
static mutex_t lock;
static uint8_t imu_data[IMU_DATA_LEN];
static Device  selected = DEV_IMU;
static Sample  sample;   // NEW

void shared_init(void) {
    mutex_init(&lock);
    // for (uint8_t i = 0; i < IMU_DATA_LEN; i++) {
    //     imu_data[i] = 0;
    // }
}

void shared_set_imu(const uint8_t data[IMU_DATA_LEN]) {
    mutex_enter_blocking(&lock);
    memcpy(imu_data, data, IMU_DATA_LEN);
    mutex_exit(&lock);
}

void shared_get_imu(uint8_t data[IMU_DATA_LEN]) {
    mutex_enter_blocking(&lock);
    memcpy(data, imu_data, IMU_DATA_LEN);
    mutex_exit(&lock);
}

void shared_set_selected(Device dev) {
    mutex_enter_blocking(&lock);
    selected = dev;
    mutex_exit(&lock);
}

Device shared_get_selected(void) {
    // If needs a mutex protection?
    return selected;
}

// NEW
void shared_publish(Device dev, const uint8_t *data) {
    // TODO
}

// NEW
bool shared_get_sample(Sample *out) {
    // TODO
    return false;
}
