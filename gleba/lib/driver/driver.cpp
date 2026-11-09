#include "driver.hpp"
#include <cstdint>
#include <hardware/i2c.h>

Imu::Imu(uint8_t address): address_(address) {

    uint8_t whoami;
    if (!this->read_byte(WHO_AM_I, &whoami)) {
        return ImuError::I2C_FAILED;
    }

bool Imu::exit_sleep() {
    return this->write_bit(PWR_MGMT_1, 6, 0);
}

bool Imu::read_byte(uint8_t address, uint8_t *data) {
    return this->read_bytes(address, 1, data);
}

bool Imu::read_bytes(uint8_t address, uint8_t length, uint8_t *data) {
    i2c_write_blocking(this->i2c, this-> address, &address, 1, true);
    int result = i2c_read_blocking(this -> i2c, this-> address, data, length, false);

    return result == length;
}

bool Imu::write_byte(uint8_t address, uint8_t data) {
    return this->write_bytes(address, 1, &data);
}

bool Imu::write_bytes(uint8_t address, uint8_t length, uint8_t *data) {
    uint8_t data_list[length + 1];
    data_list[0] = address;
    for(int i = 0; i<length; i++) {
        data_list[i+1] = data[i];
    }
    int result = i2c_write_blocking(this-> i2c, this-> address, data_list, length+1, false);
    return result == length +1;
}

bool Imu::write_bit(uint8_t address, uint8_t bit, uint8_t data) {
    uint8_t b;
    if (!this->read_byte(address, &b)) {
        return false;
    }

    b = (data != 0) ? (b | (1 << bit)) : (b & ~(1 << bit));
    return this->write_byte(address, b);
}

ImuError Imu::read_gyro(gyroReading *reading) {
    uint8_t buffer[6] = {0};
    if (!this->read_bytes(GYRO_X_HI, 6, buffer)) {
        return ImuError::I2C_FAILED;
    }
    double readings[3] = {
        (int16_t) (((int16_t) buffer[0] << 8) | buffer[1]),
        (int16_t) (((int16_t) buffer[2] << 8) | buffer[3]),
        (int16_t) (((int16_t) buffer[4] << 8) | buffer[5])
    };

    switch (this->scale_select_) {
        case FullScaleSelect::FS0:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 16.4;
            }
            break;
        case FullScaleSelect::FS1:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 32.8;
            }
            break;
        case FullScaleSelect::FS2:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 65.5;
            }
            break;
        case FullScaleSelect::FS3:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 131.0;
            }
            break;
        case FullScaleSelect::FS4:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 262.1;
            }
            break;
        case FullScaleSelect::FS5:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 524.3;
            }
            break;
        case FullScaleSelect::FS6:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 1048.6;
            }
            break;
        case FullScaleSelect::FS7:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 2097.2;
            }
            break;
        default:
            break;
    }
    reading->x = readings[0];
    reading->y = readings[1];
    reading->z = readings[2];

    return ImuError::NO_ERROR;
}

}

