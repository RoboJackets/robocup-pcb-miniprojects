//LED Controller Driver for IS31FL3254
// DataSheet for Controller : https://www.lumissil.com/assets/pdf/core/IS31FL3254_DS.pdf
// DataSheet for LEDS : https://downloads.cree-led.com/files/ds/h/HB-CLMVC-FKA.pdf

#ifndef LED_CONTROLLER_HPP
#define LED_CONTROLLER_HPP

#include "hardware/i2c.h"
#include <cstdint>

/* 
LED Controller I2C Addresses

 AD connected to GND, A2:A1 = 00;
 AD connected to VCC, A2:A1 = 11;
 AD connected to SCL, A2:A1 = 01;
 AD connected to SDA, A2:A1 = 10; 
*/
enum LEDControllerAddress {
    AD_GND = 0b01100000,
    AD_VCC = 0b01100110,
    AD_SCL = 0b01100010,
    AD_SDA = 0b01100100,
};

/*
Power Control Register - R/W
+---------+-----+------+-------+-----+-------+-----+-----+
| Bit     | D7  |  D6  | D5:D4 | D3  |  D2   | D1  | D0  |
+---------+-----+------+-------+-----+-------+-----+-----+
| Name    |  -  | RGBM | HFPS  |  -  | 14B_E | PMS | SSD |
+---------+-----+------+-------+-----+-------+-----+-----+
| Default | 0   |  0   |  00   |  0  |   0   |  0  |  0  |
+---------+-----+------+-------+-----+-------+-----+-----+
*/

#define CONTROL_REG 0x00

/*
Software Shutdown Mode

If enabled, works in Sofware Shutdown mode.
If disabled, works in Normal mode.
*/
enum SSDMode {
    ENABLE = 0,
    DISABLE = 1
};

/*
High PWM Frequency Resolution

If 8Bit, HFPS will decide the internal oscillator clock frequency and the PWM output PWM frequency.
If 10Bit, the HFP is 32kHz, LFP is 127Hz, not matter how HFPS is set.


Table 4 from IS31FL3254 DataSheet
+-----+------+-----------+----------+-----------+
| PMS | HFPS | OSC (MHZ) | LFP (HZ) | HFP (kHZ) |
+-----+------+-----------+----------+-----------+
|  1  |  xx  |     32    |    127   |    32     |
+-----+------+-----------+----------+-----------+
|     |  00  |     8     |    127   |    32     |
|     +------+-----------+----------+-----------+
|  0  |  01  |     16    |    254   |    64     |
|     +------+-----------+----------+-----------+
|     |  1x  |     32    |    508   |    128    |
+-----+------+-----------+----------+-----------+
*/
enum PMSResolution {
    R_8BIT = 0,
    R_10BIT = 1
};

/*
10+4-bit Mode

If disabled, the working mode is controlled by PMS setting.
If enabled, will enable 10+4-bit with 4-bit dithering mode.
*/
enum B14Mode {
    DISABLE = 0,
    ENABLE = 1
};

/*
High Frequency PWM Frequency Selection
*/
enum HFPS {
    F_32kHZ = 0,
    F_64kHZ = 1,
    F_128kHZ = 2
};

/*
RGB Register Mode Select 

24CH --> each of the 24 channels are controlled by it’s own PWM register.
8RGB --> pairs of 3 channels controlled by same PWM register for RGB combinations
*/
enum RGBMode {
    M_24CH = 0,
    M_8RGB = 1
};

/*
High Frequency PWM(HFP) Duty Register - R/W
OUT [24:1] high frequency PWM register byte

Table 5-1 --> 10+4-bit mode disabled
+---------+-------------------------+----------------+
|  Reg    |     02h (04h, 06h…)     | 01h (03h,05h…) |
+---------+-------+-------+---------+----------------+
|  Bit    | D7:D4 | D3:D2 |  D1:D0  |     D7:D0      |
+---------+-------+-------+---------+----------------+
|  Name   |   -   |  FMS  |  HFP_H* |     HFP_L      |
+---------+-------+-------+---------+----------------+
| Default |  0000 |  00   |   00    |    0000 0000   |
+---------+-------+-------+---------+----------------+

*Only enable in 10-bit mode

Table 5-2 --> 10+4-bit mode enabled
+---------+------------------+----------------+
|  Reg    | 02h (04h, 06h…)  | 01h (03h,05h…) |
+---------+-------+----------+----------------+
|  Bit    | D7:D6 |  D5:D0   |     D7:D0      |
+---------+-------+----------+----------------+
|  Name   |  FMS  | 14BIT_H* |    14Bit_L     |
+---------+-------+----------+----------------+
| Default |  00   | 00 0000  |   0000 0000    |
+---------+-------+----------+----------------+

*Only enable in 10+4-bit mode

HFP Registers have two registers always in consecutive order, lower bits are always the -1 register
*/

//24 Channel Mode

#define HFP1 0x02
#define HFP2 0x04
#define HFP3 0x06
#define HFP4 0x08
#define HFP5 0x0A
#define HFP6 0x0C
#define HFP7 0x1A
#define HFP8 0x1C
#define HFP9 0x1E
#define HFP10 0x20
#define HFP11 0x22
#define HFP12 0x24
#define HFP13 0x26
#define HFP14 0x28
#define HFP15 0x2A
#define HFP16 0x32
#define HFP17 0x34
#define HFP18 0x36
#define HFP19 0x38
#define HFP20 0x3A
#define HFP21 0x3C
#define HFP22 0x44
#define HFP23 0x46
#define HFP24 0x48

//8 RGB Mode

#define HFP_RGB1 0x02
#define HFP_RGB2 0x04
#define HFP_RGB3 0x0A
#define HFP_RGB4 0x0C
#define HFP_RGB5 0x0E
#define HFP_RGB6 0x12
#define HFP_RGB7 0x14
#define HFP_RGB8 0x18


/*
FMS

The FMS selects PWM Frequency Mode
HFP_LFP --> HFP + LFP
HFP_ONLY --> HFP only, LFP = 256
DC_MODE --> no PWM, output always on
CHANNEL_SHUTDOWN --> Channel shutdown mode

In 10+4-bit mode, FMS defaults to HFP_ONLY when 00 or 01.
*/
enum FMSMode {
    HFP_LFP = 0,
    HFP_ONLY = 1,
    DC_MODE = 2,
    CHANNEL_SHUTDOWN = 3
};


/*
Low Frequency PWM(LFP) Duty Register -- R/W
OUT [24:1] low frequency PWM register byte

Table 6
+---------+-----------+
|   Bit   |  D7:D0    |
+---------+-----------+
|  Name   |   LFP     |
+---------+-----------+
| Default | 0000 0000 |
+---------+-----------+
*/

#define LFP1 0x49
#define LFP2 0x4A
#define LFP3 0x4B
#define LFP4 0x4C
#define LFP5 0x4D
#define LFP6 0x4E
#define LFP7 0x55
#define LFP8 0x56
#define LFP9 0x57
#define LFP10 0x58
#define LFP11 0x59
#define LFP12 0x5A
#define LFP13 0x5B
#define LFP14 0x5C
#define LFP15 0x5D
#define LFP16 0x61
#define LFP17 0x62
#define LFP18 0x63
#define LFP19 0x64
#define LFP20 0x65
#define LFP21 0x66
#define LFP22 0x6A
#define LFP23 0x6B
#define LFP24 0x6C

/*
Update Register -- W

Updates the HFP & LFP data 

When SDB is high and SSDMode is disabled, a write of “0000 0000” to 6Dh is to update the PWM Register (01h~6Ch) values. 
*/
#define UPDATE_REG 0x6D

/*
Global Current Control Registers -- R/W

Modulates all channels DC current in 256 steps.
In 8RGB Mode, each color controls the respective color.
In 24CH Mode, each color controls a third of the 24 channels 
*/

#define GCCR 0x6E
#define GCCG 0x6F
#define GCCB 0x70

/*
Phase Delay and Clock Phase Register - R/W

+---------+-----+-----+-----+-----+-----+-----+-----+-----+
|   Bit   | D7  | D6  | D5  | D4  | D3  | D2  | D1  | D0  |
+---------+-----+-----+-----+-----+-----+-----+-----+-----+
|  Name   | PDE | HLS | PS6 | PS5 | PS4 | PS3 | PS2 | PS1 |
+---------+-----+-----+-----+-----+-----+-----+-----+-----+
| Default |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
+---------+-----+-----+-----+-----+-----+-----+-----+-----+

Phase Delay feature and Clock Phase options can work together to minimize the voltage ripple of LED power supply.
Check Phase Delay and Clock Phase section for more information 

*/

#define PHASE_REG 0x71

/*
Phase Delay Enable

IS31FL3254 features a 5 phase delay function. This bit enables or disables it.

Phase Delay separates the 24 outputs as 5 groups:
 - Group 1: 1-->6
 - Group 2: 7-->12
 - Group 3: 13-->15
 - Group 4: 16-->21
 - Group 5: 22-->24

If enabled, groups 1, 3, 4, an 5 will have a 1/(6 x freq) time delay, while group 2 will have a 2/(6 x freq) time delay
*/
enum PDE {
    DISABLE = 0,
    ENABLE = 1,
};

/*
Group Phase Delay Select

LFP --> 5 Group Phase Delay operate at low frequency PWM (LFP)
HFP --> 5 Group Phase Delay operate at high frequency PWM (HFP) 
*/
enum HLF {
    LFP = 0,
    HFP = 1,
};

/*
//TODO: figure out what this shit does

Clock Phase Select - Phase Delay and Clock Phase Register - D5:D0
*/
enum PSn {
    DISABLE = 0,
    ENABLE = 1
};


/*
Spread Spectrum Register -- R/W

+---------+-------+-----+-----+-------+-------+
|   Bit   | D7:D6 |  D5 | D4  | D3:D2 | D1:D0 |
+---------+-------+-----+-----+-------+-------+
|  Name   |   -   | SSY | SSP |  RNG  |  CLT  |
+---------+-------+-----+-----+-------+-------+
| Default |  000  |  0  |  0  |   00  |  00   |
+---------+-------+-----+-----+-------+-------+

This register enables the spread spectrum function, adjusts the cycle time and range. 
*/

#define SPREAD_SPECTRUM_REG 0x72

/*
    Spread Spectrum Synch
    
    Enable
    Disable
*/
enum SSY {
    DISABLE = 0,
    ENABLE = 1
};

/*
    Spread Spectrum Enable
    
    Enable
    Disable
*/
enum SSP {
    DISABLE = 0,
    ENABLE = 1
};

/*
Spread Spectrum Cycle Time
*/
enum CLT {
    t_500ms = 0b00,
    t_125ms = 0b01,
    t_1_4ms = 0b11,
    t_0_35ms = 0b11,
};

/*
Spread Spectrum Range 

 +- n%
*/
enum RNG {
    R_5 = 0b00,
    R_13 = 0b01,
    R_19 = 0b10,
    R_25 = 0b11,
};

/*
Reset Register - W

write of "0000 0000" to this register will reset all regsiters to their default values
*/ 

#define RESET_REG 0x7F


class LEDController {
public: 
private:
};


#endif // LED_CONTROLLER_HPP