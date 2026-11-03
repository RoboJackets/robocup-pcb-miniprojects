#include "color_sensor.hpp"

bool Color_Sensor::read_byte(uint8_t address, uint8_t *data) {
    return this->read_bytes(address, 1, data); 
}
bool Color_Sensor::read_bytes(uint8_t address, uint8_t length, uint8_t *data) {
    // Check read protocol 
    i2c_write_blocking(I2C_PORT, _address, &address, 1, true);
    i2c_read_blocking(I2C_PORT, _address, data, length, false);
    return true;
}
bool Color_Sensor::write_byte(uint8_t address, uint8_t data) {
    return this->write_bytes(address, 1, &data);
}
bool Color_Sensor::write_bytes(uint8_t address, uint8_t length, uint8_t *data) {
    uint8_t buf[length + 1];
    buf[0] = address;

    for (uint8_t i = 0; i < length; i++) {
        buf[i+1] = data[i];
    }

    int result = i2c_write_blocking(I2C_PORT, _address, buf, length, false); 
  
    return (result == length + 1); 
}
bool Color_Sensor::write_bit(uint8_t address, uint8_t bit, uint8_t data){
    uint8_t b;
    if(!this->read_byte(address, &b)) {
        return false;
    }

    b = (data != 0) ? (b | (1 << bit)) : (b & ~(1<<bit)); 

    return this->write_byte(address, b); 
}
bool Color_Sensor::write_bits(uint8_t address, uint8_t (&data)[8]){
    uint8_t b;

    if(!this->read_byte(address, &b)) {
        return false;
    }

    for (int i = 0; i < 8; i++) {
        uint8_t dataForBit = data[i]; 
        if(dataForBit != 2) b = (data != 0) ? (b | (1 << i)) : (b & ~(1<<i)); 
    }

    return this->write_byte(address, b); 
}