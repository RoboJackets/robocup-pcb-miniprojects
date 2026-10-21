#include "hall_effect.hpp"

Hall_Effect::Hall_Effect(uint8_t address, TMAG_TYPE type): _address(address) {
    uint8_t range = type == (TMAG_TYPE::A1) ? 40 : 133;
    this->xy_range = range; 
    this->z_range = range; 
}   

bool Hall_Effect::get_field_in_direction(HALL_EFFECT_DIRECTION direction, uint8_t* value) {
    uint8_t* high,* low;
    this->read_magnitude(direction, high, low);
    return this->convert_magnitude_field(high, low, value); 
}

bool Hall_Effect::read_measurement() {
    // Implement read I2C packet protocol 
}


bool Hall_Effect::read_byte(uint8_t address, uint8_t *data) {
    return this->read_bytes(address, 1, data); 
}
bool Hall_Effect::read_bytes(uint8_t address, uint8_t length, uint8_t *data) {
    // Check read protocol 
    i2c_write_blocking(I2C_PORT, _address, &address, 1, true);
    i2c_read_blocking(I2C_PORT, _address, data, length, false);
    return true;
}
bool Hall_Effect::write_byte(uint8_t address, uint8_t data) {
    return this->write_bytes(address, 1, &data);
}
bool Hall_Effect::write_bytes(uint8_t address, uint8_t length, uint8_t *data) {
    uint8_t buf[length + 1];
    buf[0] = address;

    for (uint8_t i = 0; i < length; i++) {
        buf[i+1] = data[i];
    }

    int result = i2c_write_blocking(I2C_PORT, _address, buf, length, false); 
  
    return (result == length + 1); 
}
bool Hall_Effect::write_bit(uint8_t address, uint8_t bit, uint8_t data){
    uint8_t b;
    if(!this->read_byte(address, &b)) {
        return false;
    }

    b = (data != 0) ? (b | (1 << bit)) : (b & ~(1<<bit)); 

    return this->write_byte(address, b); 
}