#include "hall_effect.hpp"

HALL_EFFECT_RESULTS::HALL_EFFECT_RESULTS() {}

Hall_Effect::Hall_Effect(): Hall_Effect(HALL_EFFECT_ADDRESS, TMAG_TYPE::A2) {}

Hall_Effect::Hall_Effect(uint8_t address, TMAG_TYPE type): _address(address) {
    uint8_t range = type == (TMAG_TYPE::A1) ? 40 : 133;
    this->xy_range = range; 
    this->z_range = range; 
}   

bool Hall_Effect::get_field_in_x_direction(uint16_t* value) {
    *value = current_results.x; 
}

bool Hall_Effect::get_field_in_y_direction(uint16_t* value) {
    *value = current_results.y; 
}
bool Hall_Effect::get_field_in_z_direction(uint16_t* value) {
   *value = current_results.z; 
}
bool Hall_Effect::get_field_in_xyz_direction(uint16_t* value) {
    value[0] = current_results.x;
    value[1] = current_results.y;
    value[2] = current_results.z; 
}
bool Hall_Effect::get_temperature(uint16_t* value) {
    *value = current_results.t; 
}
bool Hall_Effect::get_all_values(uint16_t* value) {
    value[0] = current_results.x;
    value[1] = current_results.y;
    value[2] = current_results.z; 
    value[3] = current_results.t; 
}
uint16_t Hall_Effect::merge_msb_lsb(uint8_t high, uint8_t low) {
    // Shift high 8 bits over and then add it to low
    return (uint16_t)(high << 8 | low); 
}

void Hall_Effect::disableTemperatureSensor() {
    if(this->temperatureSensorEnabled) {
        this->write_bit(T_CONFIG, 0, 0); 
    }
    this->temperatureSensorEnabled = false;
}
void Hall_Effect::enableTemperatureSensor() {
    if(!this->temperatureSensorEnabled) {
        this->write_bit(T_CONFIG, 0, 1); 
    }
    this->temperatureSensorEnabled = true;
}

void Hall_Effect::set_data(READ_TYPE readType) {
    this->read_type=readType;
    switch(this->read_type) {
        case READ_TYPE::DISABLE:
            uint8_t data[8] = {0,0,0,0, -1,-1,-1,-1}; 
            this->write_bits(SENSOR_CONFIG_1, data);
            break;
        case READ_TYPE::X:
            uint8_t data[8] = {0,0,0,1, -1,-1,-1,-1}; 
            this->write_bits(SENSOR_CONFIG_1, data);
            break;
        case READ_TYPE::Y:
            uint8_t data[8] = {0,0,1,0, -1,-1,-1,-1}; 
            this->write_bits(SENSOR_CONFIG_1, data);
            break;
        case READ_TYPE::Z:
            uint8_t data[8] = {0,1,0,0, -1,-1,-1,-1}; 
            this->write_bits(SENSOR_CONFIG_1, data);
            break;
        case READ_TYPE::XY:
            uint8_t data[8] = {0,0,1,1, -1,-1,-1,-1}; 
            this->write_bits(SENSOR_CONFIG_1, data);
            break;
        case READ_TYPE::YZ:
            uint8_t data[8] = {0,1,1,0, -1,-1,-1,-1}; 
            this->write_bits(SENSOR_CONFIG_1, data);
            break;
        case READ_TYPE::XYZ:
            uint8_t data[8] = {0,1,1,1, -1,-1,-1,-1}; 
            this->write_bits(SENSOR_CONFIG_1, data);
            break;
    }
}

void Hall_Effect::read_sensor_stream() {
    if(!interrupt) {
        switch(this->read_type) {
            case READ_TYPE::X:
                if(this->temperatureSensorEnabled) {
                    uint8_t buf[4];
                    this->read_bytes(T_MSB_RESULT, 4, buf); 
                    this->current_results.t = merge_msb_lsb(buf[0], buf[1]);
                    this->current_results.x = merge_msb_lsb(buf[2], buf[3]);
                    break;
                }
                uint8_t buf[2];
                this->read_bytes(X_MSB_RESULT, 2, buf); 
                this->current_results.x = merge_msb_lsb(buf[0], buf[1]);
                break;
            case READ_TYPE::Y:
                if(this->temperatureSensorEnabled) {
                    uint8_t buffer[2];
                    this->read_bytes(T_MSB_RESULT, 2, buffer);
                    this->current_results.t = merge_msb_lsb(buffer[0], buffer[1]); 
                }
                uint8_t buf[2];
                this->read_bytes(Y_MSB_RESULT, 2, buf); 
                this->current_results.y = merge_msb_lsb(buf[0], buf[1]);
                break;
            case READ_TYPE::Z:
                if(this->temperatureSensorEnabled) {
                    uint8_t buffer[2];
                    this->read_bytes(T_MSB_RESULT, 2, buffer);
                    this->current_results.t = merge_msb_lsb(buffer[0], buffer[1]); 
                }
                uint8_t buf[2];
                this->read_bytes(Z_MSB_RESULT, 2, buf); 
                this->current_results.z = merge_msb_lsb(buf[0], buf[1]);
                break;
            case READ_TYPE::XY:
                if(this->temperatureSensorEnabled) {
                    uint8_t buffer[2];
                    this->read_bytes(T_MSB_RESULT, 2, buffer);
                    this->current_results.t = merge_msb_lsb(buffer[0], buffer[1]); 
                }
                if(this->temperatureSensorEnabled) {
                        uint8_t buf[6];
                        this->read_bytes(T_MSB_RESULT, 6, buf); 
                        this->current_results.t = merge_msb_lsb(buf[0], buf[1]);
                        this->current_results.x = merge_msb_lsb(buf[2], buf[3]);
                        this->current_results.y = merge_msb_lsb(buf[4], buf[5]);
                        break;
                }
                uint8_t buf[4];
                this->read_bytes(X_MSB_RESULT, 4, buf); 
                this->current_results.x = merge_msb_lsb(buf[0], buf[1]);
                this->current_results.y = merge_msb_lsb(buf[2], buf[3]);
                break;
            case READ_TYPE::XYZ:
                if(this->temperatureSensorEnabled) {
                        uint8_t buf[8];
                        this->read_bytes(T_MSB_RESULT, 8, buf); 
                        this->current_results.t = merge_msb_lsb(buf[0], buf[1]);
                        this->current_results.x = merge_msb_lsb(buf[2], buf[3]);
                        this->current_results.y = merge_msb_lsb(buf[4], buf[5]);
                        this->current_results.z = merge_msb_lsb(buf[6], buf[7]);
                        break;
                }
                uint8_t buf[6];
                this->read_bytes(X_MSB_RESULT, 6, buf); 
                this->current_results.x = merge_msb_lsb(buf[0], buf[1]);
                this->current_results.y = merge_msb_lsb(buf[2], buf[3]);
                this->current_results.z = merge_msb_lsb(buf[4], buf[5]);
                break;
            case READ_TYPE::YZ:
                if(this->temperatureSensorEnabled) {
                    uint8_t buffer[2];
                    this->read_bytes(T_MSB_RESULT, 2, buffer);
                    this->current_results.t = merge_msb_lsb(buffer[0], buffer[1]); 
                }
                uint8_t buf[4];
                this->read_bytes(Y_MSB_RESULT, 4, buf); 
                this->current_results.y = merge_msb_lsb(buf[0], buf[1]);
                this->current_results.z = merge_msb_lsb(buf[2], buf[3]);
                break;
            case READ_TYPE::DISABLE:
                break; 
        }
    }
}

void Hall_Effect::interruptOff() {
    this->interrupt=false;
}
void Hall_Effect::interruptOn() {
    this->interrupt=true; 
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