#include "TempSensor.hpp"
#include <cstdint>
#include <hardware/i2c.h>

TempSensor::TempSensor(i2c_inst_t *bus, uint8_t address)
    : bus_(bus), address_(address)
{
    i2c_init(bus_, 100 * 1000);
}

bool TempSensor::readTemp(uint8_t reading[2])
{
    
}
