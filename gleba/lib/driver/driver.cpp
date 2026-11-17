#include <stdio.h>
#include "driver.hpp"
#include "pico/stdlib.h"   // sleep_ms

#define REG_DATA_START   0x1F   // first of the 12 data bytes
#define REG_PWR_MGMT0    0x4E   // power: write 0x0F = accel + gyro on
#define REG_WHO_AM_I     0x75   // always reads 0x42
#define WHO_AM_I_VALUE   0x42
#define I2C_TIMEOUT_US   2000

static i2c_inst_t *imu_i2c;
static uint8_t     imu_addr;

// Send the register number, then read len bytes starting there.
static bool read_regs(uint8_t reg, uint8_t *buf, size_t len) {
    int w = i2c_write_timeout_us(imu_i2c, imu_addr, &reg, 1, true,  I2C_TIMEOUT_US);
    int r = i2c_read_timeout_us(imu_i2c, imu_addr, buf,  len, false, I2C_TIMEOUT_US);
    return (w == 1) && (r == (int)len);
}

// Write one value into one register: send {reg, value}.
static bool write_reg(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    int w = i2c_write_timeout_us(imu_i2c, imu_addr, data, 2, false, I2C_TIMEOUT_US);
    return w == 2;
}

bool imu_init(i2c_inst_t *i2c, uint8_t addr) {
    imu_i2c = i2c;
    imu_addr = addr;
    uint8_t id;
    if (!read_regs(REG_WHO_AM_I, &id, 1) || id != WHO_AM_I_VALUE) {
        return false;
    }
    if (!write_reg(REG_PWR_MGMT0, 0X0F))
        return false;
    sleep_ms(30);
    return true;
}

bool imu_read_raw(uint8_t out[IMU_RAW_LEN]) {
    return read_regs(REG_DATA_START, out, IMU_RAW_LEN);
}