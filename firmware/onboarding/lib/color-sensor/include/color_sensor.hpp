#pragma once

#include "pico/types.h"
#include "hardware/i2c.h"

#define COLOR_SENSOR_ADDRESS 0x28

#define I2C_PORT i2c0 // IDK
#define I2C_SDA 0 // Change this
#define I2C_SCL 1 // Change this


/**
 * @brief Driver for VEML3328
 * @author Topher Fontana
 * @date Fall 2026 
 */
class Color_Sensor {
public:
    Color_Sensor();
    Color_Sensor(uint8_t addr); 
private:
    uint8_t _address; 
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
    /**
     * @brief Write to given bits of an address provided
     * 
     * @param address The address to write to
     * @param data The data to write into the bits MUST BE 8 IN LENGTH (Use -1 to not write to something+)
     * @return True if the operation was successful
     */
    bool write_bits(uint8_t address, uint8_t (&data)[8]);
};