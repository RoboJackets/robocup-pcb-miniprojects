# Daisy-Chained Sensor Suite Firmware

## Overview

You will be implementing the main control loop of your electrical team's PCB and writing the driver for ONE of the six I2C peripherals.

## Requirements

1. SPI daisy-chain I/O
    - You must read the input from the previous board, add your information, and then write to the next board.
    - Only write after a input trigger but always output most recent data
    - Must match SPI and Packet specifications
        - Board ID bit is equal to the numerical placement of your device in the data packet (i.e. temperature board will set bit 2). Bit 0 is used for the debug input board
2. Assigned Driver
    - Must implement as much functionality as possible for geneneric use
        - This will be more than is needed than for just the data packet
        - Will be evaluated per device
    - Must be well documented
        - Should be able to be implemented by the other teams without much effort
3. Interface
    - You have two buttons and three LEDs. Find some way to use them to choose between the six I2C devices and show which one is currently selected
        - Make sure to keep tabs on how they are being placed by your electrical
4. Overall
    - You must use the Pico SDK
    - No external libraries (you are allowed built in libraries such as hardware/i2c, pico/multicore, etc)
    - You must utilize both of the cores
        - Recommended split is SPI on one core, devices on the other, but it is up to you
        - Data safety and control flow must be maintained between cores (i.e. Mutexes)
    - Limit polling as much as possible to lower power draw
    - You must document your implementation in some way and keep it in your team's drive folder
    - In general have clean and easy to read code and a solid git history


## Specifications

### Inter-Board SPI

- Frequency: 1Mhz
- Mode: 3
- IN bus: SPI0
- OUT bus: SPI1

### Inter-Board Data Packet

56 Bytes Total

1. IMU: 12 Bytes total
    - AccX 2 Bytes
    - AccY 2 Bytes
    - AccZ 2 Bytes
    - GyroX 2 Bytes
    - GyroY 2 Bytes
    - GyroZ 2 Bytes
2. Temperature: 2 Bytes total
    - Temp 2 Bytes
3. Hall Effect: 6 Bytes total
    - MagX 2 Bytes
    - MagY 2 Bytes
    - MagZ 2 Bytes
4. GPIO Expander: 1 Byte total
    - Inputs (7..0) 1 Byte
5. Color Sensor: 10 Bytes total
    - R 2 Bytes
    - G 2 Bytes
    - B 2 Bytes
    - C 2 Bytes
    - IR 2 Bytes
6. LED Controller: 24 Bytes total
    - LED N 3 Bytes
7. Board Checksum: 1 Byte total
    - Board ID bit (7..0) 1 Byte


| Byte | Absolute Bits | Section | Field |
|---:|:---:|---|---|
| 0 | 7..0 | IMU | AccX (high) |
| 1 | 15..8 | IMU | AccX (low) |
| 2 | 23..16 | IMU | AccY (high) |
| 3 | 31..24 | IMU | AccY (low) |
| 4 | 39..32 | IMU | AccZ (high) |
| 5 | 47..40 | IMU | AccZ (low) |
| 6 | 55..48 | IMU | GyroX (high) |
| 7 | 63..56 | IMU | GyroX (low) |
| 8 | 71..64 | IMU | GyroY (high) |
| 9 | 79..72 | IMU | GyroY (low) |
| 10 | 87..80 | IMU | GyroZ (high) |
| 11 | 95..88 | IMU | GyroZ (low) |
| 12 | 103..96 | Temperature | Temp (high) |
| 13 | 111..104 | Temperature | Temp (low) |
| 14 | 119..112 | Hall Effect | MagX (high) |
| 15 | 127..120 | Hall Effect | MagX (low) |
| 16 | 135..128 | Hall Effect | MagY (high) |
| 17 | 143..136 | Hall Effect | MagY (low) |
| 18 | 151..144 | Hall Effect | MagZ (high) |
| 19 | 159..152 | Hall Effect | MagZ (low) |
| 20 | 167..160 | GPIO Expander | Inputs (7..0) |
| 21 | 175..168 | Color Sensor | R (high) |
| 22 | 183..176 | Color Sensor | R (low) |
| 23 | 191..184 | Color Sensor | G (high) |
| 24 | 199..192 | Color Sensor | G (low) |
| 25 | 207..200 | Color Sensor | B (high) |
| 26 | 215..208 | Color Sensor | B (low) |
| 27 | 223..216 | Color Sensor | C (high) |
| 28 | 231..224 | Color Sensor | C (low) |
| 29 | 239..232 | Color Sensor | IR (high) |
| 30 | 247..240 | Color Sensor | IR (low) |
| 31 | 255..248 | LED Controller | LED 0 R |
| 32 | 263..256 | LED Controller | LED 0 G |
| 33 | 271..264 | LED Controller | LED 0 B |
| 34 | 279..272 | LED Controller | LED 1 R |
| 35 | 287..280 | LED Controller | LED 1 G |
| 36 | 295..288 | LED Controller | LED 1 B |
| 37 | 303..296 | LED Controller | LED 2 R |
| 38 | 311..304 | LED Controller | LED 2 G |
| 39 | 319..312 | LED Controller | LED 2 B |
| 40 | 327..320 | LED Controller | LED 3 R |
| 41 | 335..328 | LED Controller | LED 3 G |
| 42 | 343..336 | LED Controller | LED 3 B |
| 43 | 351..344 | LED Controller | LED 4 R |
| 44 | 359..352 | LED Controller | LED 4 G |
| 45 | 367..360 | LED Controller | LED 4 B |
| 46 | 375..368 | LED Controller | LED 5 R |
| 47 | 383..376 | LED Controller | LED 5 G |
| 48 | 391..384 | LED Controller | LED 5 B |
| 49 | 399..392 | LED Controller | LED 6 R |
| 50 | 407..400 | LED Controller | LED 6 G |
| 51 | 415..408 | LED Controller | LED 6 B |
| 52 | 423..416 | LED Controller | LED 7 R |
| 53 | 431..424 | LED Controller | LED 7 G |
| 54 | 439..432 | LED Controller | LED 7 B |
| 55 | 447..440 | Board Checksum | Board ID bits (7..0)|