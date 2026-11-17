#pragma once
// Data shared between core 0 and core 1. Every function locks one mutex
// internally, so callers never touch the mutex directly.
// Owner: Yueqi Mars

#include <stdint.h>
// #include "devices.hpp"   // enum Device moved there

#define IMU_DATA_LEN 12   // AccX, AccY, AccZ, GyroX, GyroY, GyroZ (int16, high byte first)

// NEW: one reading, labelled with the device it came from.
// struct Sample {
//     Device  dev;                     // device this data was read from
//     uint8_t len;                     // number of bytes used in data[]
//     bool    valid;                   // false: nothing read from the selected device yet
//     uint8_t data[MAX_SECTION_LEN];
// };

// Call once in main(), BEFORE multicore_launch_core1().
void shared_init(void);

// Latest IMU sample. Core 1 writes, core 0 reads.
void shared_set_imu(const uint8_t data[IMU_DATA_LEN]);
void shared_get_imu(uint8_t data[IMU_DATA_LEN]);

// // Device currently selected with the buttons.
// void   shared_set_selected(Device dev);
// Device shared_get_selected(void);

// // NEW: core 1 stores the newest reading of `dev`.
// void shared_publish(Device dev, const uint8_t *data);

// // NEW: core 0 copies the newest sample into *out. Returns false if there is
// // no valid sample.
// bool shared_get_sample(Sample *out);
