#pragma once

#include "hardware/i2c.h"
#include "pico/stdlib.h"

/**
 * @brief Driver for TMAG5273 (A2 or A1)
 */

/**
 * @brief Hall Effect Sensor I2C Address
 */
#define HALL_EFFECT_ADDRESS 0x35

/**
 * @brief The device config 1 register is in charge of device stuff (sampling)
 */
#define DEVICE_CONFIG_1 0x00
/**
 * | Bits | Field                    | Values                                              |
 * |------|--------------------------|-----------------------------------------------------|
 * | 7    | CRC enabled              | 0 = disabled, 1 = enabled                           |
 * | 6:5  | Mag temperature coeff.   |                                                     |
 * | 4:2  | Additional sampling      | 0 = 1x, 1 = 2x, 2 = 4x, 3 = 8x, 4 = 16x, 5 = 32x    |
 * | 1:0  | I2C read mode            | 0 = standard 3-byte read                            |
 * |      |                          | 1 = 1-byte read command for 16-bit sensor data      |
 * |      |                          | 2 = 1-byte I2C read for 8-bit MSB data + conversion |
 */

 /**
 * @brief The device config 2 register is in charge of device stuff (thresholds)
 */
#define DEVICE_CONFIG_2 0x01
/**
- | Bits | Field          | Values                                                    |
- |------|----------------|-----------------------------------------------------------|
- | 7:5  | Thresholds     | 0 = 2's complement of each x_THR to create threshold      |
- |      |                | 1 = 7 MSB of x_THR to create two opposite thresholds      |
- | 4    | Mode switch    | 0 = low current, 1 = low noise                            |
- | 3    | Glitch filter  | 0 = glitch on, 1 = glitch off                             |
- | 2    | Conversion pin | 0 = Default, 1 = INT sig                                  |
- | 1:0  | Operating Mode | 0 = standby, 1 = sleep, 2 = continuous measure, 3 = W&S   |
 */

/** 
 * @brief The sensor config register in charge of changing channels
 */
#define SENSOR_CONFIG_1 0x02
/**
- | Bits | Field              | Values                                                   |
- |------|--------------------|----------------------------------------------------------|
- | 7:4  | Mag Channel Enable | 0 = Off, 1 = X, 2 = Y, 3 = XY, 4 = Z, 5 = ZX, 6 = YZ     |
- |      |                    | 7 = XYZ, 8 = XYX, 9 = YXY, A = YZY, B = XZX              |
- | 3:0  | Sleeptime          | 0 = 1ms, 1 = 5ms, 2 = 10ms, 3 = 15ms, 4 = 20ms, etc.     |
 */

/**
 * @brief Sensor config register in charge of setting range & other stuff
 */
#define SENSOR_CONFIG_2 0x03
/**
- | Bits | Field                 | Values                                  |
- |------|-----------------------|-----------------------------------------|
- | 7    | R                     | —                                       |
- | 6    | Threshold Count       | —                                       |
- | 5    | Interrupt Above/Below | —                                       |
- | 4    | Mag Gain Correction   | —                                       |
- | 3:2  | Angle Calculation     | 0 = No, 1 = (X,Y), 2 = (Y-Z), 3 = (X,Z) |
- | 1    | XY Range              | 0 = ± Standard Range, 1 = ± 2x Standard |
- | 0    | Z Range               | —                                       |
 */

// CONFIGS

/**
 * @brief Config register for threshold for X axis
 */
#define X_THR_CONFIG 0x04
/**
 * @brief Config register for threshold for Y axis
 */
#define Y_THR_CONFIG 0x05
/**
 * @brief Config register for threshold for Z axis
 */
#define Z_THR_CONFIG 0x06
/**
- | Bits | Field                      | Values / Description                                      |
- |------|----------------------------|-----------------------------------------------------------|
- | 7:0  | A_THR_CONFIG (A = X, Y, Z) | 2's complement threshold code (+- 128 range)              |
- |      |                            | 0 = no threshold comparison                               |
- |      |                            | Threshold = 133(1 + Range) / 128 * Config (swap for Z)    |
 */
 
/**
 * @brief Config register for threshold for Temperature 
 */
#define T_CONFIG 0x07
/**
- | Bits | Field   | Values / Description                                      |
- |------|---------|-----------------------------------------------------------|
- | 7:1  | Config  | 0 = no threshold, -41C = 0x1A, 170C = 0x#4, 8 C/LSB res.       |
- | 0    | Channel | 0 = disabled, 1 = enabled                                 |
 */

/**
 * @brief Config register for interrupt configuration
 */
#define INT_CONFIG 0x08


/**
 * @brief I2C address register
 */
#define I2C_ADDRESS_REG 0x0C 
/**
- | Bits | Field  | Values / Description                 |
- |------|--------|--------------------------------------|
- | 7:1  | Addy   | 35 = reset                           |
- | 0    | Update | 0 = no, 1 = update with first 7 bits |
 */

 /**
  * @brief The device ID register is in charge of storing the device ID
  */
 #define DEVICE_ID 0x0D

 /**
  * @brief The manufacturer ID register is in charge of storing the manufacturer ID's LSB
  */
#define MANUFACTURER_ID_LSB 0x0E
/**
 * @brief The manufacturer ID register is in charge of storing the manufacturer ID's MSB
 */
#define MANUFACTURER_ID_MSB 0x0F

/**
 * @brief Register storing MSB for temperature sensor
 */
#define T_MSB_RESULT 0X10
/**
 * @brief Register storing LSB for temperature sensor
 */
#define T_LSB_RESULT 0x11
/**
- | Bits | Field | Values / Description          |
- |------|-------|-------------------------------|
- | 15:8 | MSB   | Most Significant Byte         |
- | 7:0  | LSB   | Least Significant Byte        |
 */

 // XYZ Registers

#define X_MSB_RESULT 0x12
#define X_LSB_RESULT 0x13

#define Y_MSB_RESULT 0x14
#define Y_LSB_RESULT 0x15

#define Z_MSB_RESULT 0x16
#define Z_LSB_RESULT 0x17
/**
- CORRESPONDING MAGNETIC FIELD REGISTERS
- | Bits | Field | Values / Description |
- |------|-------|----------------------|
- | 15:8 | MSB   | MSB (Magnetic Field) |
- | 7:0  | LSB   | LSB (Magnetic Field) |
 */

#define CONV_STATUS 0x18

/**
 * @brief Angle measurements registers
 * @note 13LSB after combining MSB and LSB, 4LSB bits are in xxxx/16
 */

#define ANGLE_RESULT_MSB 0x19
#define ANGLE_RESULT_LSB 0x1A 

#define MAGNITUDE_RESULT 0x1B

#define DEVICE_STATUS 0x1C
/**
- Device status register
- | Bits | Field            | Values / Description |
- |------|------------------|----------------------|
- | 7:5  | Reserved         | —                    |
- | 4    | INT              | —                    |
- | 3    | Oscillator Error | —                    |
- | 2    | INT Error        | —                    |
- | 1    | CRC Error        | —                    |
- | 0    | VCC UV           | —                    |
 */

enum class TMAG_TYPE {
    A1,
    A2
};

/**
 * @brief Read type of sensor you want to use
 * Temperature is kept seperate since it's not our main function
 */
enum class READ_TYPE: uint8_t {
    X,Y,Z,XY,YZ,XYZ,DISABLE,ZX
}; 

/**
 * @brief Keep Track of Hall Effect Results 
 * 
 */
class HALL_EFFECT_RESULTS {
public:
    int16_t x = 0, y = 0, z = 0; // Magnetic Fields (in millitesla) / SB
    int16_t t = 0; // Temperature (in celsius) / SB

    HALL_EFFECT_RESULTS(
        int16_t x,
        int16_t y,
        int16_t z,
        int16_t t
    ) : x(x), y(y), z(z), t(t) {} 

    HALL_EFFECT_RESULTS() = default; 
};

/**
 * @brief Class for a TMAG5273 Hall Effect Sensor (configurable for A-D 1 OR 2 Models)
 * @author Topher Fontana
 * @date Fall 2026
 */
class Hall_Effect {
public:    
    /**
     * @brief Disable the sensor (put in sleep mode)
     * @return true If the sensor was successfully disabled, false otherwise
     */
    bool disableSensor(); 

    /**
     * @brief Enable the sensor (wake from sleep mode)
     * @return true If the sensor was successfully enabled, false otherwise
     */
    bool enableSensor();

    /**
     * @brief Enable the temperature sensor
     * @return true If the temperature sensor was successfully enabled, false otherwise
     */
    bool enableTemperatureSensor();
    /**
     * @brief Disable the temperature sensor
     * @return true If the temperature sensor was successfully disabled, false otherwise
     */ 
    bool disableTemperatureSensor(); 

    /**
     * @brief The most updated results of the Hall Effect Sensor
     */
    HALL_EFFECT_RESULTS current_results; 

    /**
     * @brief Construct a new Hall_Effect object
     * 
     * @param address The I2C address of the device
     * @param type The type (A1 or A2) of the sensor
     * @param i2c_number Which i2c bus to use (0 or 1)
     * @param i2c_sda_pin Which SDA pin you are using
     * @param i2c_scl_pin Which SCL pin you are using
     * @param int_pin The pin to use for the interrupt (if you want to use it, don't) 
     */
    Hall_Effect(uint8_t address, TMAG_TYPE type, uint8_t i2c_number, uint8_t i2c_sda_pin, uint8_t i2c_scl_pin, uint8_t int_pin = 0xFF); 
    /**
     * @brief Construct a new Hall_Effect object with default stuff if you're lazy (like me)
     */
    Hall_Effect(); 
    /**
     * @brief Initalize Hall Effect sensor 
     * 
     * @return true Hall Effect properly setup
     * @return false Hall Effect not properly setup, check wiring MFG ID not correct
     */
    bool init_hall_effect(); 

    /**
     * @brief Set the data object
     * 
     * @param readType The type of data to read from the sensor (X, Y, Z, XY, YZ, XYZ, DISABLE, ZX)
     * @return true If the data was successfully set, false otherwise
     */
    bool set_data(READ_TYPE readType);  

    /**
     * @brief Actual value of magnetic field in mT at X direction
     *
     * @param value 
     * @return true 
     * @return false 
     */
    bool get_field_in_x_direction(float* value);
    /**
     * @brief Actual value of magnetic field in mT at Y direction
     *
     * @param value 
     * @return true 
     * @return false 
     */
    bool get_field_in_y_direction(float* value);
    /**
     * @brief Actual value of magnetic field in mT at Z direction
     *
     * @param value 
     * @return true 
     * @return false 
     */
    bool get_field_in_z_direction(float* value);
    /**
     * @brief Actual value of magnetic field in mT in XYZ directions
     *
     * @param value A pointer to store the values in (length = 3)
     * @return true 
     * @return false 
     */
    bool get_field_in_xyz_direction(float* value);
    /**
     * @brief Value of the temperature sensor in Celsuis / SB
     *
     * @param value A int16_t pointer to store the temperature in
     * @return true 
     * @return false 
     */
    bool get_raw_temperature(int16_t* value);
    /**
     * @brief Actual value of the temperature sensor in Celsuis 
     *
     * @param value A float pointer to store the temperature in
     * @return true 
     * @return false 
     */
    bool get_temperature(float* value);
    /**
     * @brief Actual value of magnetic field in mT in XYZ directions and temperature in Celsuis
     *
     * @param value A pointer to store the values in (length = 4)
     * @return true 
     * @return false 
     */
    bool get_all_values(float* value);
    /**
     * @brief Main function, is required to be in main update loop if you want polling
     * @return true If the read was successful, false otherwise
     */
    bool read_sensor_stream();

    /**
     * @brief Turn off interrupt mode (not acc used on sensor suite)
     * @return true If the interrupt was successfully disabled, false otherwise
     */
    bool interruptOff();
    /**
     * @brief Turn on interrupt mode (not acc used on sensor suite)
     * @param pin The pin to use for the interrupt (if you want to use it, don't)
     * @note If you don't provide a pin, it will use the one provided in the constructor
     * @return true If the interrupt was successfully enabled, false otherwise
     */
    bool interruptOn(uint8_t pin = 0xFF); 
    /**
     * @brief Increase the range of the sensor via a write 
     * @return true If the range was successfully increased, false otherwise
     * @note If the range is already at the maximum, it will not increase further (and return true)
     */
    bool increaseRange();
    /**
     * @brief Decrease the range of the sensor via a write 
     * @return true If the range was successfully decreased, false otherwise
     * @note If the range is already at the minimum, it will not decrease further (and return true)
     */
    bool decreaseRange(); 
    /**
     * @brief Interrupt pending? 
     */
    bool isInterruptPending();
private:
    /**
     * @brief I2C address of the devices
     */
    const uint8_t _address;

    i2c_inst* i2c = i2c0; 
    uint8_t i2c_sda;
    uint8_t i2c_scl; 
    uint8_t int_pin = 0xFF; 

    READ_TYPE read_type = READ_TYPE::XYZ; 

    /**
     * @brief Enable interrupt mode (if set to false will continously poll)
     */
    bool interrupt = false;
     /**
     * @brief Is the temperature sensor enabled, will read it if used
     */
    bool temperatureSensorEnabled = false; 

    volatile bool interrupt_pending = false;

    static Hall_Effect* interrupt_instance;
    static void gpio_irq_handler(uint gpio, uint32_t events);

    /**
     * @brief Current XY range of the sensor 
     * @note Standard range is +- of 40 mT (A1) or +- of 133 mT (A2)
     */
    uint16_t xy_range; 

    /**
     * @brief Current Z range of the sensor 
     * @note Standard range is +- of 40 mT (A1) or +- of 133 mT (A2)
     */
    uint16_t z_range;

    /**
     * @brief Read from a given address
     * 
     * @param address The address to read from
     * @param data The output data 
     */
    bool read_byte(uint8_t address, uint8_t *data);

    /**
     * @brief Read a number of bytes starting from an address
     * 
     * @param address The start address
     * @param length The number of bytes to read
     * @param data The buffer to store the output into
     */
    bool read_bytes(uint8_t address, uint8_t length, uint8_t *data);

    /**
     * @brief write a byte to a given address 
     * 
     * @param address The address on the IMU to write to
     * @param data The data to write to the address
     */
    bool write_byte(uint8_t address, uint8_t data);

    /**
     * @brief Write a number of bytes starting at a given address
     * 
     * @param address The address on the IMU to start writing to
     * @param length The number of bytes to write
     * @param data The data to write to the IMU
     */
    bool write_bytes(uint8_t address, uint8_t length, uint8_t *data);

    /**
     * @brief Write to a given bit of an address provided
     * 
     * @param address The address to write to
     * @param bit The bit number to write to
     * @param data The data to write into the bit
     * @return true if successful, false otherwise
     */
    bool write_bit(uint8_t address, uint8_t bit, uint8_t data);
    /**
     * @brief Convert MSB and LSB from Hall Effect to actual field values (mT)
     * 
     * @param high MSB Data
     * @param low LSB Data
     * @return The value 
     */
    int16_t merge_msb_lsb(uint8_t high, uint8_t low);
    /**
     * @brief Converts the raw value from the sensor to mT based on the range of the sensor
     * 
     * @param value Raw value from the sensor
     * @param range Range of the sensor
     * @return float The value in mT
     */
    float convert_to_mT(int16_t value, uint16_t range);
    /**
     * @brief Writes to a field of an address provided
     * 
     * @param address Address to write to
     * @param mask Mask to apply
     * @param value Value to write
     * @return true 
     * @return false 
     */
    bool write_field(uint8_t address, uint8_t mask, uint8_t value);
  };