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
    if (reading == NULL)
    {
        return false;
    }
    // tell the sensor that we want to access the reading
    uint8_t reg = 0x00;
    if (i2c_write_blocking(bus_, address_, &reg, 1, true) != 1)
    {
        return false;
    }

    // read the response
    uint8_t response[2];
    if (i2c_read_blocking(bus_, address_, response, 2, false) != 2)
    {
        return false;
    }

    // write the response into reading if successful
    reading[0] = response[0];
    reading[1] = response[1];

    return true;
}
