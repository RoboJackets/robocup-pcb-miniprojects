#pragma once

#include <cstdint>
#include <hardware/i2c.h>

class TempSensor {
public:
  /**
   * Constructor for the temperature sensor
   *
   * @param bus: the instance of the i2c bus; the RP2040 has 2 I2C controllers
   * which are defined by the Pico SDK as i2c0 and i2c1
   * @param address: the I2C address of the physical device (0x48 if pins A0-A2
   * are set to GND)
   */
  TempSensor(i2c_inst_t *bus, uint8_t address = 0x48);

  /**
   * Reads the temperature given by the temperature sensor
   *
   * @param reading: an array of 2 bytes to read to
   *
   * @return true if the read was successful, false otherwise
   */
  bool readTemp(uint8_t reading[2]);

private:
  i2c_inst_t *bus_;
  uint8_t address_;
};
