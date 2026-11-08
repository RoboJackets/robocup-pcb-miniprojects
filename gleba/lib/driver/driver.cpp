#include "driver.hpp"

// Driver state (one IMU per board).
static i2c_inst_t *imu_i2c  = nullptr;
static uint8_t     imu_addr = IMU_DEFAULT_ADDR;

// ---- Core ----
bool imu_init(i2c_inst_t *i2c, uint8_t addr) {
    imu_i2c  = i2c;
    imu_addr = addr;
    // TODO: read WHO_AM_I, soft reset, power on accel + gyro
    return false;
}

bool imu_read(uint8_t out[12]) {
    // TODO: burst-read ACCEL_DATA_X1 .. GYRO_DATA_Z0 (12 bytes)
    return false;
}

// ---- Readings ----
bool imu_read_accel(int16_t xyz[3])  { return false; }   // TODO
bool imu_read_gyro(int16_t xyz[3])   { return false; }   // TODO
bool imu_read_temp_c(float *celsius) { return false; }   // TODO
float imu_accel_to_g(int16_t raw)    { return 0.0f; }    // TODO
float imu_gyro_to_dps(int16_t raw)   { return 0.0f; }    // TODO

// ---- Configuration ----
bool imu_set_accel_config(ImuAccelRange range, ImuOdr odr) { return false; }  // TODO
bool imu_set_gyro_config(ImuGyroRange range, ImuOdr odr)   { return false; }  // TODO
bool imu_soft_reset(void) { return false; }   // TODO
bool imu_sleep(void)      { return false; }   // TODO
bool imu_wake(void)       { return false; }   // TODO

// ---- FIFO ----
bool imu_fifo_enable(bool enable)            { return false; }  // TODO
int  imu_fifo_count(void)                    { return -1; }     // TODO
int  imu_fifo_read(uint8_t *buf, size_t len) { return -1; }     // TODO

// ---- Low-level register access ----
bool imu_read_regs(uint8_t reg, uint8_t *buf, size_t len) { return false; }  // TODO
bool imu_write_reg(uint8_t reg, uint8_t value)            { return false; }  // TODO
