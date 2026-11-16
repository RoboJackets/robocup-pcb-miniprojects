#include "driver.hpp"
#include <cstdint>
#include <hardware/i2c.h>

Imu::Imu(uint8_t address): address_(address) {}

ImuError Imu::begin(LowPassFilter filter, FullScaleSelect scale_select, FullScaleSelect_Acc acc_scale_select) {

    this-> low_pass_ = filter;
    this-> scale_select_ = scale_select;
    this-> scale_select_acc_ = acc_scale_select;

    uint8_t whoami;
    //reads the value inside register and stores it @ whoami
    if (!this->read_byte(WHO_AM_I_REGISTER, &whoami)) {
        return ImuError::I2C_FAILED;
    }
    // who am i contains the default value, if they are unequal then return an error
    if(whoami != WHO_AM_I) {
        return ImuError::INVALID_WHOAMI;
    }

    uint8_t configuration;
    // setting low pass value
    if(!this->read_byte(CONFIGURATION, &configuration)) {
        return ImuError::LOW_PASS_FILTER_ERR;
    }
    //setting the configuration equal to the filter value passed in the constructor
    configuration|= filter; 
    if(!this->write_byte(CONFIGURATION, configuration)) {
        return ImuError::LOW_PASS_FILTER_ERR;
    }
    

    //setting scale select for gyroscope
    uint8_t gyro_configuration;
    if(!this->read_byte(GYRO_CONFIG, &gyro_configuration)) {
        return ImuError::GYRO_CONFIG_ERR;
    }
    gyro_configuration|= scale_select;
    if(!this->write_byte(GYRO_CONFIG, gyro_configuration)) {
        return ImuError::GYRO_CONFIG_ERR;
    }


    //setting scale select for the accelerator
    uint8_t acc_configuration;
    if(!this->read_byte(ACCEL_CONFIG, &acc_configuration)) {
        return ImuError::ACC_CONFIG_ERR;
    }
    acc_configuration |= acc_scale_select;
    if(!this->write_byte(ACCEL_CONFIG, gyro_configuration)) {
        return ImuError::ACC_CONFIG_ERR;
    }
    return ImuError::NO_ERROR;
}

    

bool Imu::exit_sleep() {
    return this->write_bit(PWR_MGMT, 6, 0);
}

bool Imu::read_byte(uint8_t address, uint8_t *data) {
    return this->read_bytes(address, 1, data);
}

bool Imu::read_bytes(uint8_t address, uint8_t length, uint8_t *data) {
    //verify here if you should use i2c0 or i2c
    i2c_write_blocking(i2c0, this-> address_, &address_, 1, true);
    int result = i2c_read_blocking(i2c0, this-> address_, data, length, false);

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
    int result = i2c_write_blocking(i2c0, this-> address_, data_list, length+1, false);
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

ImuError Imu::read_acc(accReading *reading) {
    uint8_t buffer[6] = {0};
    if (!this->read_bytes(ACC_X_HI, 6, buffer)) {
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
                readings[i] = readings[i] * 1000 / 2048;
            }
            break;
        case FullScaleSelect::FS1:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 4096;
            }
            break;
        case FullScaleSelect::FS2:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 8192;
            }
            break;
        case FullScaleSelect::FS3:
            for (size_t i = 0; i < 3; i++) {
                readings[i] = readings[i] * 1000 / 16384;
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



