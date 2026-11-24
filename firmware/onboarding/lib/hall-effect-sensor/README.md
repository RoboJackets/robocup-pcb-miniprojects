# Hall Effect Driver Library for TMAG5273

## Initialization
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
In your main.cpp use it like so
```c++
if(!sensor.read_sensor_stream()); // ERROR 
```
To actually make I2C calls to the sensor

```c++
uint16_t values[4];
sensor.get_all_values(values); 
```
The above is an example of getting all values of the sensor

All units will be in milli Tesla / SB or Celsius / SB

```c++
uint16_t values[4];
sensor.get_converted_values(values); 
```

The above is an example of getting all values of the sensor THAT ARE CONVERTED TO USEFUL UNITS 

All units will be in milli Tesla or Celsius 

## Errors
If anything returns false, check the doxygen comments for what tf went wrong 