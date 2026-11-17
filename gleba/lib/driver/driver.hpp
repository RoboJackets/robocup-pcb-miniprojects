#pragma once
// Minimal ICM-42605 driver: turn the sensors on and read raw data.
// Uses the chip's default settings: +-2000 dps, +-16 g, 1 kHz.
// The caller sets up the I2C pins and calls i2c_init() first.

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

#define IMU_ADDR_DEFAULT 0x68   // AD0 pin tied to GND
#define IMU_RAW_LEN      12     // AccX, AccY, AccZ, GyroX, GyroY, GyroZ (int16, high byte first)

// Check that the IMU answers and turn accel + gyro on. Returns false on failure.
bool imu_init(i2c_inst_t *i2c, uint8_t addr);

// Read the 12 raw data bytes. Returns false on I2C error.
bool imu_read_raw(uint8_t out[IMU_RAW_LEN]);