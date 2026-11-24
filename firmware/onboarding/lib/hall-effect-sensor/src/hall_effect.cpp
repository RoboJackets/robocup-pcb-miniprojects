#include "hall_effect.hpp"

Hall_Effect* Hall_Effect::interrupt_instance = nullptr;

void Hall_Effect::gpio_irq_handler(uint gpio, uint32_t events) {
    if (interrupt_instance == nullptr) {
        return;
    }

    if (gpio == interrupt_instance->int_pin && (events & GPIO_IRQ_EDGE_FALL)) {
        interrupt_instance->interrupt_pending = true;
    }
}


bool Hall_Effect::increaseRange() {
    if ((this->xy_range == 266 && this->z_range == 266) || (this->xy_range == 80 && this->z_range == 80)) {
        return true; 
    }
    if(!this->write_bit(SENSOR_CONFIG_2, 1, 1)) {
        return false; 
    }
    if(!this->write_bit(SENSOR_CONFIG_2, 0, 1)) {
        return false; 
    }
    return this->write_field(SENSOR_CONFIG_2, 0x03, 0x03);
}

bool Hall_Effect::decreaseRange() {
    if ((this->xy_range == 40 && this->z_range == 40) || (this->xy_range == 80 && this->z_range == 80)) {
        return true; 
    }
    if(!this->write_bit(SENSOR_CONFIG_2, 1, 0)) {
        return false; 
    }
    if(!this->write_bit(SENSOR_CONFIG_2, 0, 0)) {
        return false; 
    }
    return this->write_field(SENSOR_CONFIG_2, 0x03, 0x00);
}

Hall_Effect::Hall_Effect(): Hall_Effect(HALL_EFFECT_ADDRESS, TMAG_TYPE::A2, 0, 1, 2) {}

Hall_Effect::Hall_Effect(uint8_t address, TMAG_TYPE type, uint8_t i2c_number, uint8_t i2c_sda_pin, uint8_t i2c_scl_pin, uint8_t int_pin): _address(address) {
    uint8_t range = type == (TMAG_TYPE::A1) ? 40 : 133;
    if (int_pin != 0xFF) {
        this->interrupt = false; 
        this->int_pin = int_pin; 
    }
    else {
        this->interrupt = false; 
        this->int_pin = 0xFF;
    }
    i2c = (i2c_number == 0) ? i2c0 : i2c1;
    i2c_sda = i2c_sda_pin;
    i2c_scl = i2c_scl_pin; 
    this->xy_range = range; 
    this->z_range = range; 
}   

bool Hall_Effect::init_hall_effect() {
    // Setup I2C
    i2c_init(i2c, 100 * 1000); 
    gpio_set_function(i2c_sda, GPIO_FUNC_I2C);
    gpio_set_function(i2c_scl, GPIO_FUNC_I2C);
    gpio_pull_up(i2c_sda);
    gpio_pull_up(i2c_scl);
    
    sleep_us(300); // wait for bootup

    uint8_t mfg_id_lsb, mfg_id_msb;
    if (!this->read_byte(MANUFACTURER_ID_LSB, &mfg_id_lsb) ||
        !this->read_byte(MANUFACTURER_ID_MSB, &mfg_id_msb)) {
        return false;
    }

    // Checks if its 0x5449
    if (mfg_id_lsb != 0x49 || mfg_id_msb != 0x54) {
        return false;
    }

    if (!this->write_field(DEVICE_CONFIG_2, 0x03, 2)) {
        return false;
    }
    return this->set_data(READ_TYPE::XYZ); 
}

float Hall_Effect::convert_to_mT(int16_t value, uint16_t range) {
    return (float)value * range / 32768.0f;
}

bool Hall_Effect::disableSensor() {
    return this->write_field(DEVICE_CONFIG_2, 0x03, 0x01); 
}

bool Hall_Effect::enableSensor() {
    uint8_t buf; 
    if (i2c_read_blocking(i2c, _address, &buf, 1, false) != 1) { // wake up the sensor
        return false;
    }
    sleep_us(300); // wait for bootup
    return this->write_field(DEVICE_CONFIG_2, 0x03, 0x02);
}

bool Hall_Effect::get_field_in_x_direction(float* value) {
    if(value == nullptr) {
        return false; 
    }
    *value = convert_to_mT(current_results.x, this->xy_range); 
    return true; 
}

bool Hall_Effect::get_field_in_y_direction(float* value) {
    if(value == nullptr) {
        return false; 
    }
    *value = convert_to_mT(current_results.y, this->xy_range); 
    return true; 
}
bool Hall_Effect::get_field_in_z_direction(float* value) {
    if(value == nullptr) {
        return false; 
    }
   *value = convert_to_mT(current_results.z, this->z_range); 
   return true; 
}
bool Hall_Effect::get_field_in_xyz_direction(float* value) {
    if(value == nullptr) {
        return false; 
    }
    value[0] = convert_to_mT(current_results.x, this->xy_range); 
    value[1] = convert_to_mT(current_results.y, this->xy_range); 
    value[2] = convert_to_mT(current_results.z, this->z_range);
    return true;
}
bool Hall_Effect::get_raw_temperature(int16_t* value) {
    // T_SENST0 = 25 C 
    // T_ADCT0 = 17508
    // T_ADC_RES 58 LSB/C
    if (value == nullptr || !temperatureSensorEnabled) {
        return false; 
    }
    *value = current_results.t; 
    return true;
}
bool Hall_Effect::get_temperature(float* value) {
    if (value == nullptr || !temperatureSensorEnabled) {
        return false; 
    }
    *value = (current_results.t - 17508) / 58.0f + 25.0f; 
    return true;
}
bool Hall_Effect::get_all_values(float* value) {
    if (value == nullptr) return false;
    value[0] = convert_to_mT(current_results.x, this->xy_range);
    value[1] = convert_to_mT(current_results.y, this->xy_range);
    value[2] = convert_to_mT(current_results.z, this->z_range);
    value[3] = (current_results.t - 17508) / 58.0f + 25.0f;
    return true;
}
int16_t Hall_Effect::merge_msb_lsb(uint8_t high, uint8_t low) {
    // Shift high 8 bits over and then add it to low
    return (int16_t)(high << 8 | low); 
}

bool Hall_Effect::disableTemperatureSensor() {
    if(this->temperatureSensorEnabled) {
        if(!this->write_bit(T_CONFIG, 0, 0)) {
            return false;
        }
        this->temperatureSensorEnabled = false;
    }
    return true;
}
bool Hall_Effect::enableTemperatureSensor() {
    if(!this->temperatureSensorEnabled) {
        if(!this->write_bit(T_CONFIG, 0, 1)) {
            return false; 
        } 
        this->temperatureSensorEnabled = true;
    }
    return true; 
}

bool Hall_Effect::set_data(READ_TYPE readType) {
    switch(readType) {
        case READ_TYPE::X:
            if(!this->write_field(SENSOR_CONFIG_1, 0xF0, 1 << 4)) {
                return false;
            }
            this->read_type = readType; 
            return true;
        case READ_TYPE::Y:
            if(!this->write_field(SENSOR_CONFIG_1, 0xF0, 2 << 4)) {
                return false;
            }
            this->read_type = readType;
            return true;
        case READ_TYPE::Z:
            if(!this->write_field(SENSOR_CONFIG_1, 0xF0, 4 << 4)) {
                return false;
            }
            this->read_type = readType;
            return true;
        case READ_TYPE::XY:
            if(!this->write_field(SENSOR_CONFIG_1, 0xF0, 3 << 4)) {
                return false;
            }
            this->read_type = readType;
            return true;
        case READ_TYPE::ZX:
            if(!this->write_field(SENSOR_CONFIG_1, 0xF0, 5 << 4)) {
                return false;
            }
            this->read_type = readType;
            return true;
        case READ_TYPE::YZ:
            if(!this->write_field(SENSOR_CONFIG_1, 0xF0, 6 << 4)) {
                return false;
            }
            this->read_type = readType;
            return true;
        case READ_TYPE::XYZ:
            if(!this->write_field(SENSOR_CONFIG_1, 0xF0, 7 << 4)) {
                return false;
            }
            this->read_type = readType;
            return true;
        case READ_TYPE::DISABLE:
            if(!this->write_field(SENSOR_CONFIG_1, 0xF0, 0 << 4)) {
                return false;
            }
            this->read_type = readType;
            return true;
    }

    return false; 
}
bool Hall_Effect::read_sensor_stream() {
    if (interrupt) {
        if (!interrupt_pending || gpio_get(int_pin)) {
            return false;
        }
    }

    // I'm lazy
    const bool t = temperatureSensorEnabled;
    uint8_t b[8];

    switch (read_type) {
        case READ_TYPE::X:
            if (t) {
                if (!read_bytes(T_MSB_RESULT, 4, b)) return false;
                current_results.t = merge_msb_lsb(b[0], b[1]);
                current_results.x = merge_msb_lsb(b[2], b[3]);
            } else {
                if (!read_bytes(X_MSB_RESULT, 2, b)) return false;
                current_results.x = merge_msb_lsb(b[0], b[1]);
            }
            break;

        case READ_TYPE::Y:
            if (t) {
                if (!read_bytes(T_MSB_RESULT, 6, b)) return false;
                current_results.t = merge_msb_lsb(b[0], b[1]);
                current_results.y = merge_msb_lsb(b[4], b[5]);
            } else {
                if (!read_bytes(Y_MSB_RESULT, 2, b)) return false;
                current_results.y = merge_msb_lsb(b[0], b[1]);
            }
            break;

        case READ_TYPE::Z:
            if (t) {
                if (!read_bytes(T_MSB_RESULT, 8, b)) return false;
                current_results.t = merge_msb_lsb(b[0], b[1]);
                current_results.z = merge_msb_lsb(b[6], b[7]);
            } else {
                if (!read_bytes(Z_MSB_RESULT, 2, b)) return false;
                current_results.z = merge_msb_lsb(b[0], b[1]);
            }
            break;

        case READ_TYPE::XY:
            if (t) {
                if (!read_bytes(T_MSB_RESULT, 6, b)) return false;
                current_results.t = merge_msb_lsb(b[0], b[1]);
                current_results.x = merge_msb_lsb(b[2], b[3]);
                current_results.y = merge_msb_lsb(b[4], b[5]);
            } else {
                if (!read_bytes(X_MSB_RESULT, 4, b)) return false;
                current_results.x = merge_msb_lsb(b[0], b[1]);
                current_results.y = merge_msb_lsb(b[2], b[3]);
            }
            break;

        case READ_TYPE::YZ:
            if (t) {
                if (!read_bytes(T_MSB_RESULT, 8, b)) return false;
                current_results.t = merge_msb_lsb(b[0], b[1]);
                current_results.y = merge_msb_lsb(b[4], b[5]);
                current_results.z = merge_msb_lsb(b[6], b[7]);
            } else {
                if (!read_bytes(Y_MSB_RESULT, 4, b)) return false;
                current_results.y = merge_msb_lsb(b[0], b[1]);
                current_results.z = merge_msb_lsb(b[2], b[3]);
            }
            break;

        case READ_TYPE::ZX:
            if (t) {
                if (!read_bytes(T_MSB_RESULT, 8, b)) return false;
                current_results.t = merge_msb_lsb(b[0], b[1]);
                current_results.x = merge_msb_lsb(b[2], b[3]);
                current_results.z = merge_msb_lsb(b[6], b[7]);
            } else {
                if (!read_bytes(X_MSB_RESULT, 6, b)) return false;
                current_results.x = merge_msb_lsb(b[0], b[1]);
                current_results.z = merge_msb_lsb(b[4], b[5]);
            }
            break;

        case READ_TYPE::XYZ:
            if (t) {
                if (!read_bytes(T_MSB_RESULT, 8, b)) return false;
                current_results.t = merge_msb_lsb(b[0], b[1]);
                current_results.x = merge_msb_lsb(b[2], b[3]);
                current_results.y = merge_msb_lsb(b[4], b[5]);
                current_results.z = merge_msb_lsb(b[6], b[7]);
            } else {
                if (!read_bytes(X_MSB_RESULT, 6, b)) return false;
                current_results.x = merge_msb_lsb(b[0], b[1]);
                current_results.y = merge_msb_lsb(b[2], b[3]);
                current_results.z = merge_msb_lsb(b[4], b[5]);
            }
            break;

        case READ_TYPE::DISABLE:
            if (!t) return false;
            if (!read_bytes(T_MSB_RESULT, 2, b)) return false;
            current_results.t = merge_msb_lsb(b[0], b[1]);
            break;
        default:
            // FUCK YOU DOING DUMBASS I LITERALLY DECLARE IT FOR YOU 
            return false;
    }

    if (interrupt) {
       interrupt_pending = !gpio_get(int_pin);
    }

    return true;
}

bool Hall_Effect::interruptOff() {
    if (this->int_pin != 0xFF) {
        gpio_set_irq_enabled(this->int_pin, GPIO_IRQ_EDGE_FALL, false);
    }

    if (!this->write_byte(INT_CONFIG, 0x00)) {
        if (int_pin != 0xFF) {
            gpio_set_irq_enabled(int_pin, GPIO_IRQ_EDGE_FALL, true);
        }
        return false;
    }

    this->interrupt = false;
    this->interrupt_pending = false;

    if (this->interrupt_instance == this) {
        this->interrupt_instance = nullptr;
    }

    return true;
}
bool Hall_Effect::interruptOn(uint8_t pin) {
    if (pin == 0xFF && this->int_pin == 0xFF) {
        return false; 
    }
    this->int_pin = (pin == 0xFF) ? this->int_pin : pin;  
    
    gpio_init(this->int_pin);
    gpio_set_dir(this->int_pin, GPIO_IN);

    if(!this->write_byte(INT_CONFIG, 0x84)) {
        return false; 
    }

    interrupt_instance = this;

    gpio_set_irq_enabled_with_callback(this->int_pin, GPIO_IRQ_EDGE_FALL, true, &Hall_Effect::gpio_irq_handler);

    this->interrupt_pending = false;

    this->interrupt=true; 

    return true;
}
bool Hall_Effect::read_byte(uint8_t address, uint8_t *data) {
    return this->read_bytes(address, 1, data); 
}
bool Hall_Effect::read_bytes(uint8_t address, uint8_t length, uint8_t *data) {
    // Check read protocol 
    if (i2c_write_blocking(i2c, _address, &address, 1, true) != 1) return false;
    return i2c_read_blocking(i2c, _address, data, length, false) == length;
}
bool Hall_Effect::write_byte(uint8_t address, uint8_t data) {
    return this->write_bytes(address, 1, &data);
}
bool Hall_Effect::write_bytes(uint8_t address, uint8_t length, uint8_t *data) {
    if(length > 8) {
        return false; 
    }

    uint8_t buf[length + 1];
    buf[0] = address;

    for (uint8_t i = 0; i < length; i++) {
        buf[i+1] = data[i];
    }

    return i2c_write_blocking(i2c, _address, buf, length + 1, false) == length + 1; 
}
bool Hall_Effect::write_bit(uint8_t address, uint8_t bit, uint8_t data) {
    uint8_t b;
    if (!this->read_byte(address, &b)) {
        return false; 
    }
    b = (b & ~(1 << bit)) | ((data & 1) << bit);
    return this->write_byte(address, b);
}
bool Hall_Effect::write_field(uint8_t address, uint8_t mask, uint8_t value) {
    uint8_t b;
    if (!this->read_byte(address, &b)) {
        return false; 
    }
    b = (b & ~mask) | (value & mask);
    return this->write_byte(address, b);
}