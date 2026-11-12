#include "dummy.hpp"

int8_t init_imu(i2c_inst_t* i2c_port, uint8_t addr) {
    return 0;
}

int8_t read_imu(uint8_t (&buf)[12]) {
    return 0;
}

int8_t init_temp(i2c_inst_t* i2c_port, uint8_t addr) {
    return 0;
}

int8_t read_temp(uint8_t (&buf)[2]) {
    return 0;
}

int8_t init_hall(i2c_inst_t* i2c_port, uint8_t addr) {
    return 0;
}

int8_t read_hall(uint8_t (&buf)[6]) {
    return 0;
}

int8_t init_gpiox(i2c_inst_t* i2c_port, uint8_t addr) {
    return 0;
}

int8_t read_gpiox(uint8_t (&buf)[1]) {
    return 0;
}

int8_t init_color(i2c_inst_t* i2c_port, uint8_t addr) {
    return 0;
}

int8_t read_color(uint8_t (&buf)[10]) {
    return 0;
}

int8_t init_led(i2c_inst_t* i2c_port, uint8_t addr) {
    return 0;
}

int8_t write_led(uint8_t (&buf)[24]) {
    return 0;
}
