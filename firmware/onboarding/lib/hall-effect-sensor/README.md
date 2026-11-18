# Hall Effect Driver Library for TMAG5273

## Initialization
===
```c++
Hall_Effect sensor = Hall_Effect(uint8_t address, TMAG_TYPE type, uint8_t i2c_number, uint8_t i2c_sda_pin, uint8_t i2c_scl_pin); 
sensor.init_hall_effect(); 
```

This creates a `Hall_Effect` sensor object with the input parameters:
- Address: The I2C address you think it's on
- Type: The type (A1 or A2) of TMAG5273 sensor it is
- i2c_number: The I2C bus (0 or 1) you want to use
- i2c_sda_pin: The SDA pin
- i2c_scl_pin: The SCL pin

## Usage
===


## Errors
=== 
