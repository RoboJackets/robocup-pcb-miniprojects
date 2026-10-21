#pragma once

#include "pico/types.h"
#include "hardware/i2c.h"

/**
 * @brief Driver for TMAG5273 aka 
 */

#define HALL_EFFECT_ADDRESS 0x35

#define I2C_PORT i2c0 // IDK
#define I2C_SDA 0 // Change this
#define I2C_SCL 1 // Change this

// Configure device operation modes

#define DEVICE_CONFIG_1 0x00
/**
 * | 7 CRC Enabled | 6-5 Mag Temperature Coefficient |
 * | 4-2 Additional sampling (0 - 1x, 1 - 2x ... 5 - 32x) |
 * | 1-0 I2C Read Mode (0 - Standard 3 byte Read, 1 - 1 byte read commmand for 16 bit
 * sensor data, 2 - 1 byte I2C read for 8 bit MSB data and conversion)
 */
 
#define DEVICE_CONFIG_2 0x01
/**
 * | 7-5 Thresholds (0 - 2s complement of each x_THR to create magnetic threshold, 
 * 1 - 7SLB of x_THR to create two opposite magnetic thresholds) |
 * | 4 Mode switch (0 - low current, 1 - low noise) | 
 * | 3 Glitch filter (0 - glitch on, 1 - glitch off) | 
 * | 2 Conversion pin (0 - Default, 1 - INT sig) |
 * | 1-0 Operating Mode (0 - standby, 1 - sleep, 2 - continous measure, 3 - W&S)
 */

/** 
 * @brief The sensor config register in charge of changing channels
 */
#define SENSOR_CONFIG_1 0x02
/**
 * | 7-4 Mag Channel Enable (0 - Off, 1 - X, 2 - Y, 3 - XY, 4 - Z, 5 - ZX, 6 - YZ,
 * 7 - XYZ, 8 - XYX, 9 - YXY, A - YZY, B - XZX) |
 * | 3-0 Sleeptime (0 - 1ms, 1 - 5ms, 2 - 10ms, 3 - 15 ms, 4 - 20ms, Look at sheet)
 */
 
/**
 * @brief Sensor config register in charge of setting range & other stuff
 */
#define SENSOR_CONFIG_2 0x03
/**
 * | 7 R | 6 Threshold Count | 5 Interrupt Above/Below | 4 Mag Gain Correction |
 * | 3-2 Angle Calculation (0 - No, 1 - (X,Y), 2 - (Y-Z), 3 - (X,Z)) |
 * | 1 XY Range (0 - +- Standard Range, 1 - +- 2x Standard) |
 * | 0 Z Range 
 */

// CONFIGS

#define X_THR_CONFIG 0x04
#define Y_THR_CONFIG 0x05
#define Z_THR_CONFIG 0x06
/**
 * | 7-0 A_THR_CONFIG (A is XYZ) 
 * 2's complement axis threshold code for limit check
 * Range is +- 128
 * 0 means no threshold comparison 
 * Threshold value is 133(1+X_Y_RANGE)/128 * X_THR_CONFIG (X and Y, swap for Z for Z) |
 */
 
#define T_CONFIG 0x07
/**
 * | 7-1 Config (0 - no threshold, -41C = 1, 170C = 34, 8 degree C/LSB resolution) |
 * | 0 Channel (0 - disabled, 1 - enabled) | 
 */

// I2C ADDRESS REGISTER

#define I2C_ADDRESS_REG 0x0C 
/**
 * | 7-1 Addy (35 - reset) | 
 * | 0 Update (0 - no, 1 - update with first 7 bits) | 
 */

// T REGISTERS

#define T_MSB_RESULT 0X10
#define T_LSB_RESULT 0x11
/*
 * | D15-D8 MSB | D7-D0 LSB | 
 */

 // XYZ Registers

#define X_MSB_RESULT 0x12
#define X_LSB_RESULT 0x13

#define Y_MSB_RESULT 0x14
#define Y_LSB_RESULT 0x15

#define Z_MSB_RESULT 0x16
#define Z_LSB_RESULT 0x17
/*
 *  CORRESPONDING MAGNETIC FIELD REGISTERS
 *  | D15-D8 MSB | D7-D0 LSB | 
 */

#define CONV_STATUS 0x18

/**
 * @brief Angle measurements registers
 * @note 13LSB after combining MSB and LSB, 4LSB bits are in xxxx/16
 */

#define ANGLE_RESULT_MSB 0x19
#define ANGLE_RESULT_LSB 0x1A 

#define MAGNITUDE_RESULT 0x1B

/**
 * @brief Device status register
 * 
 * @note | 7-5 Reserved | 4 INT | 3 Oscillator Error | 2 INT Error | 1 CRC Error | 0 VCC UV | 
 */
#define DEVICE_STATUS 0x1C

enum HALL_EFFECT_ERROR {
    UNDERVOLTAGE,
    CRC_FAILURE, // Cyclical Redundancy Check (can be enabled)
    INT_PIN_ERROR,
    OSCILLATOR_ERROR 
};

enum HALL_EFFECT_DIRECTION {
    X,Y,Z
}; 

enum TMAG_TYPE {
    A1,
    A2
};

enum HALL_EFFECT_MEASUREMENT_TYPE {
    XYZ,
    X,
    Y,
    Z,
    XY,
};

/**
 * @brief Class for a TMAG5273 Hall Effect Sensor (configurable for A-D 1/2 Models)
 * @author Topher Fontana
 * @date Fall 2026
 */
class Hall_Effect {
    /*
    All [ MSB LSB ]
    MagX - 2 Bytes 
    MagY - 2 Bytes
    MagZ - 2 Bytes
    */

    /**
     * @brief Current XY range of the sensor 
     * @note Standard range is +- of 40 mT (A1) or +- of 133 mT (A2)
     */
    uint8_t xy_range; 


    /**
     * @brief Current Z range of the sensor 
     * @note Standard range is +- of 40 mT (A1) or +- of 133 mT (A2)
     */
    uint8_t z_range; 
   
    /**
     * @brief I2C address of the device 
     * 
     */
    const uint8_t _address;

    /**
     * @brief Is the temperature sensor enabled, will read it if used
     */
    bool temperatureSensorEnabled = false; 

    Hall_Effect(uint8_t address, TMAG_TYPE type); 
    /**
     * @brief Initalize Hall Effect sensor 
     * 
     * @return true Hall Effect properly setup
     * @return false Hall Effect not properly setup
     */
    bool init_hall_effect(); 

    /**
     * @brief Read magnitude of a given direction
     * 
     * @param direction Direction in X, Y, or Z axis
     * @param high Data structure to store MSB in 
     * @param low Data structure to store LSB in 
     * @return true Read success
     * @return false Read fail
     */
    bool read_magnitude(HALL_EFFECT_DIRECTION direction, uint8_t* high, uint8_t* low); 
    
    /**
     * @brief Convert MSB and LSB from Hall Effect to actual field values (mT)
     * 
     * @param high MSB Data
     * @param low LSB Data
     * @param value Value to store result in
     * @return true Read success
     * @return false Read fail 
     */
    bool convert_magnitude_field(uint8_t* high, uint8_t* low, uint8_t* value);

    /**
     * @brief Actual value of magnetic field in mT at direction
     * 
     * @param direction 
     * @param value 
     * @return true 
     * @return false 
     */
    bool get_field_in_direction(HALL_EFFECT_DIRECTION direction, uint8_t* value);

    // Add angle & temperature stuff

private:
    bool exit_sleep(); 

    HALL_EFFECT_MEASUREMENT_TYPE measurementType; 

    bool read_measurement(); 

    /**
     * @brief Read from a given address
     * 
     * @param address The address to read from
     * @param data The output data 
     * @return True if the operation was successful
     */
    bool read_byte(uint8_t address, uint8_t *data);

    /**
     * @brief Read a number of bytes starting from an address
     * 
     * @param address The start address
     * @param length The number of bytes to read
     * @param data The buffer to store the output into
     * @return True if the operation was successful
     */
    bool read_bytes(uint8_t address, uint8_t length, uint8_t *data);

    /**
     * @brief write a byte to a given address 
     * 
     * @param address The address on the IMU to write to
     * @param data The data to write to the address
     * @return True if the operation was successful
     */
    bool write_byte(uint8_t address, uint8_t data);

    /**
     * @brief Write a number of bytes starting at a given address
     * 
     * @param address The address on the IMU to start writing to
     * @param length The number of bytes to write
     * @param data The data to write to the IMU
     * @return True if the operation was successful
     */
    bool write_bytes(uint8_t address, uint8_t length, uint8_t *data);

    /**
     * @brief Write to a given bit of an address provided
     * 
     * @param address The address to write to
     * @param bit The bit number to write to
     * @param data The data to write into the bit
     * @return True if the operation was successful
     */
    bool write_bit(uint8_t address, uint8_t bit, uint8_t data);
};

