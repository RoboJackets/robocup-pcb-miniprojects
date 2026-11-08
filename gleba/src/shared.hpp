#pragma once
// Data shared between core 0 and core 1. Every function locks one mutex
// internally, so callers never touch the mutex directly.
// Owner: Yueqi (agree before changing)

#include <stdint.h>

#define IMU_DATA_LEN 12   // AccX, AccY, AccZ, GyroX, GyroY, GyroZ (int16, high byte first)

// The six I2C devices on the board (selected with the buttons)
enum Device : uint8_t {
    DEV_IMU = 0,
    DEV_TEMP,
    DEV_HALL,
    DEV_GPIO,    // GPIO expander + DIP switch
    DEV_COLOR,
    DEV_LED,     // LED driver
    DEV_COUNT
};

// Call once in main(), BEFORE multicore_launch_core1().
void shared_init(void);

// Latest IMU sample. Core 1 writes, core 0 reads.
void shared_set_imu(const uint8_t data[IMU_DATA_LEN]);
void shared_get_imu(uint8_t data[IMU_DATA_LEN]);

// Device currently selected with the buttons. Core 1 writes, anyone reads.
void   shared_set_selected(Device dev);
Device shared_get_selected(void);
