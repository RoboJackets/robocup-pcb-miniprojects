#include "shared.hpp"

#include <string.h>
#include "pico/sync.h"

// static: only this file can see these, so every access goes through the
// functions below (which always take the lock).
static mutex_t lock;
static uint8_t imu_data[IMU_DATA_LEN];
static Device  selected = DEV_IMU;

void shared_init(void) {
    // TODO: mutex_init(&lock); clear imu_data
}

void shared_set_imu(const uint8_t data[IMU_DATA_LEN]) {
    // TODO: lock -> memcpy into imu_data -> unlock
}

void shared_get_imu(uint8_t data[IMU_DATA_LEN]) {
    // TODO: lock -> memcpy out of imu_data -> unlock
}

void shared_set_selected(Device dev) {
    // TODO: lock -> selected = dev -> unlock
}

Device shared_get_selected(void) {
    // TODO: lock -> copy selected -> unlock -> return copy
    return selected;
}
