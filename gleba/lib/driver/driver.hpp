#pragma once
// ICM-42605 6-axis IMU driver (I2C), Pico SDK only.
// Reusable by any board: the caller sets up the I2C bus (pins + i2c_init)
// and passes the controller in. No pins are hard-coded here.
// Owner: Meghna
//
// Fixed interface used by core 1 (do not change without telling the team):
//   imu_init(), imu_read()
// Everything else below is a suggested full-feature API; Meghna can adjust it.

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hardware/i2c.h"

#define IMU_ADDR_AD0_LOW   0x68   // our board (AP_AD0 = GND)
#define IMU_ADDR_AD0_HIGH  0x69
#define IMU_DEFAULT_ADDR   IMU_ADDR_AD0_LOW
#define IMU_WHO_AM_I_VALUE 0x42

// ---- Configuration values (check codes against the ICM-42605 datasheet) ----
enum ImuAccelRange { IMU_ACCEL_16G, IMU_ACCEL_8G, IMU_ACCEL_4G, IMU_ACCEL_2G };
enum ImuGyroRange  { IMU_GYRO_2000DPS, IMU_GYRO_1000DPS, IMU_GYRO_500DPS, IMU_GYRO_250DPS,
                     IMU_GYRO_125DPS, IMU_GYRO_62_5DPS, IMU_GYRO_31_25DPS, IMU_GYRO_15_625DPS };
enum ImuOdr        { IMU_ODR_8KHZ, IMU_ODR_4KHZ, IMU_ODR_2KHZ, IMU_ODR_1KHZ, IMU_ODR_500HZ,
                     IMU_ODR_200HZ, IMU_ODR_100HZ, IMU_ODR_50HZ, IMU_ODR_25HZ, IMU_ODR_12_5HZ };

// ---- Core (used by core 1) ----

// Check WHO_AM_I, reset, turn accel + gyro on with default settings.
// Returns false if the device does not answer or WHO_AM_I != 0x42.
bool imu_init(i2c_inst_t *i2c, uint8_t addr);

// Raw packet data: AccX, AccY, AccZ, GyroX, GyroY, GyroZ, each int16 high byte
// first (matches packet bytes 0-11). Returns false on I2C error.
bool imu_read(uint8_t out[12]);

// ---- Readings ----
bool imu_read_accel(int16_t xyz[3]);          // raw counts
bool imu_read_gyro(int16_t xyz[3]);           // raw counts
bool imu_read_temp_c(float *celsius);
float imu_accel_to_g(int16_t raw);            // uses the current range
float imu_gyro_to_dps(int16_t raw);           // uses the current range

// ---- Configuration ----
bool imu_set_accel_config(ImuAccelRange range, ImuOdr odr);
bool imu_set_gyro_config(ImuGyroRange range, ImuOdr odr);
bool imu_soft_reset(void);
bool imu_sleep(void);                         // accel + gyro off
bool imu_wake(void);                          // accel + gyro on (low-noise mode)

// ---- FIFO ----
bool imu_fifo_enable(bool enable);
int  imu_fifo_count(void);                    // bytes waiting, or -1 on error
int  imu_fifo_read(uint8_t *buf, size_t len); // bytes read, or -1 on error

// ---- Low-level register access ----
bool imu_read_regs(uint8_t reg, uint8_t *buf, size_t len);
bool imu_write_reg(uint8_t reg, uint8_t value);
