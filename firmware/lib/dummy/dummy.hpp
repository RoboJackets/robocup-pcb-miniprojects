// Dummy folder to hold filler functions to be replaced with real libs
#ifndef DUMMY
#define DUMMY

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <cstdint>

/**
 * Initializes the IMU
 * 
 * @param i2c_port I2C port to use for communication
 * @param addr I2C address of IMU
 * 
 * @return Returns -1 on error, 0 on success
 * 
 * @note I2C port must be configured seperately
 */
int8_t init_imu(i2c_inst_t* i2c_port, uint8_t addr);

/**
 * Reads IMU info
 * 
 * @param buf 12 Byte buffer to be filled by IMU
 * 
 * @return Returns -1 on error, 0 on success
 */
int8_t read_imu(uint8_t (&buf)[12]);

/**
 * Initializes the temp sensor
 * 
 * @param i2c_port I2C port to use for communication
 * @param addr I2C address of temp sensor
 * 
 * @return Returns -1 on error, 0 on success
 * 
 * @note I2C port must be configured seperately
 */
int8_t init_temp(i2c_inst_t* i2c_port, uint8_t addr);

/**
 * Reads temp info
 * 
 * @param buf 2 Byte buffer to be filled by temp sensor
 * 
 * @return Returns -1 on error, 0 on success
 */
int8_t read_temp(uint8_t (&buf)[2]);

/**
 * Initializes the hall sensor
 * 
 * @param i2c_port I2C port to use for communication
 * @param addr I2C address of hall sensor
 * 
 * @return Returns -1 on error, 0 on success
 * 
 * @note I2C port must be configured seperately
 */
int8_t init_hall(i2c_inst_t* i2c_port, uint8_t addr);

/**
 * Reads hall info
 * 
 * @param buf 6 Byte buffer to be filled by hall sensor
 * 
 * @return Returns -1 on error, 0 on success
 */
int8_t read_hall(uint8_t (&buf)[6]);

/**
 * Initializes the GPIO expander
 * 
 * @param i2c_port I2C port to use for communication
 * @param addr I2C address of GPIO expander
 * 
 * @return Returns -1 on error, 0 on success
 * 
 * @note I2C port must be configured seperately
 */
int8_t init_gpiox(i2c_inst_t* i2c_port, uint8_t addr);

/**
 * Reads GPIO expander info
 * 
 * @param buf 1 Byte buffer to be filled by GPIO expander
 * 
 * @return Returns -1 on error, 0 on success
 */
int8_t read_gpiox(uint8_t (&buf)[1]);

/**
 * Initializes the color sensor
 * 
 * @param i2c_port I2C port to use for communication
 * @param addr I2C address of color sensor
 * 
 * @return Returns -1 on error, 0 on success
 * 
 * @note I2C port must be configured seperately
 */
int8_t init_color(i2c_inst_t* i2c_port, uint8_t addr);

/**
 * Reads color info
 * 
 * @param buf 10 Byte buffer to be filled by color sensor
 * 
 * @return Returns -1 on error, 0 on success
 */
int8_t read_color(uint8_t (&buf)[10]);

/**
 * Initializes the led driver
 * 
 * @param i2c_port I2C port to use for communication
 * @param addr I2C address of led driver
 * 
 * @return Returns -1 on error, 0 on success
 * 
 * @note I2C port must be configured seperately
 */
int8_t init_led(i2c_inst_t* i2c_port, uint8_t addr);

/**
 * Write to LEDs
 * 
 * @param buf 24 Byte buffer to be written to the LEDs
 * 
 * @return Returns -1 on error, 0 on success
 */
int8_t write_led(uint8_t (&buf)[24]);

#endif //DUMMY