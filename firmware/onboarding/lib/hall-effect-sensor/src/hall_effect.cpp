#include "hall_effect.hpp"

HALL_EFFECT_RESULTS::HALL_EFFECT_RESULTS() {}

Hall_Effect::Hall_Effect(): Hall_Effect(HALL_EFFECT_ADDRESS, TMAG_TYPE::A2) {}

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
    // Set the current measurement type that we want to check
    if(this->prev_measurementType != this->measurementType) {
        this->prev_measurementType = this->measurementType;

        switch(measurementType) {
            case X:
                break;
            case Y:
                break;
            case Z:
                break;
            case XY:
                break;
            case YZ:
                // 0,1,2,3, 4-7
                uint8_t bits[8] = {-1,-1,-1,-1, 0,1,1,0}; 
                this->write_bits(SENSOR_CONFIG_1, bits) ;
                break; 
            case XYZ: 
                break;
        }
    }
    if (this->prev_temperatureSensorEnabled != this->temperatureSensorEnabled) {
        this->prev_temperatureSensorEnabled = this->temperatureSensorEnabled; 
        this->write_bit(T_CONFIG, 0, 1); 
    }

    
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
bool Hall_Effect::write_bits(uint8_t address, uint8_t (&data)[8]){
    uint8_t b;

    if(!this->read_byte(address, &b)) {
        return false;
    }

    for (int i = 0; i < 8; i++) {
        uint8_t dataForBit = data[i]; 
        if(dataForBit != -1) b = (data != 0) ? (b | (1 << i)) : (b & ~(1<<i)); 
    }

    return this->write_byte(address, b); 
}