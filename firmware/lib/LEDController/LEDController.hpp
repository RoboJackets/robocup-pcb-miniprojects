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

//Spread Spectrum -- R/W
#define SPREAD_SPECTRUM_REG 0x72

//Reset -- W
#define RESET_REG 0x7F

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
Software Shutdown Mode - Control Register - Bit D0

If enabled, works in Sofware Shutdown mode.
If disabled, works in Normal mode.
*/
enum SSDMode {
    ENABLE = 0,
    DISABLE = 1
};

/*
High PWM frequency Resolution - Control Register - Bit D1

If 8Bit, HFPS setting will decide the internal oscillator clock frequency and the PWM output PWM frequency.
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
10+4-bit Mode - Control Register - Bit D2

If disabled, the working mode is controlled by PMS setting.
If enabled, will enable 10+4-bit with 4-bit dithering mode.
*/
enum B14Mode {
    DISABLE = 0,
    ENABLE = 1
};

/*
High Frequency PWM frequency selection - Control Register - Bit D5:D4
*/
enum HFPS {
    F_32kHZ = 0,
    F_64kHZ = 1,
    F_128kHZ = 2
};

/*
RGB Register Mode Select - Control Register - Bit D6

24CH --> each of the 24 channels are controlled by it’s own PWM register.
8RGB --> pairs of 3 channels controlled by same PWM register for RGB combinations
*/
enum RGBMode {
    M_24CH = 0,
    M_8RGB = 1
};

/*
High Frequency PWM(HFP) Duty Register -- R/W
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

*/

//TODO: organize this better
#define HFP_1 0x01
#define HFP_1_end 0x0C
#define HFP_2 0x19
#define HFP_2_end 0x2A
#define HFP_3 0x31
#define HFP_3_end 0x3C
#define HFP_4 0x43
#define HFP_4_end 0x48


/*
FMS - HFP Duty Register - 14B Disabled --> D3:D2, 14B Enabled --> D7:D6

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

#define LFP_1 0x49
#define LFP_1_end 0x4E
#define LFP_2 0x55
#define LFP_2_end 0x5D
#define LFP_3 0x61
#define LFP_3_end 0x66
#define LFP_4 0x6A
#define LFP_4_end 0x6C

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
*/

#define PHASE_REG 0x71

/*
Phase Delay Enable - Phase Delay and Clock Phase Register - D7

IS31FL3254 features a 5 phase delay function. This bit enables or disables it.
*/
enum PDE {
    DISABLE = 0,
    ENABLE = 1,
};

/*
Group Phase Delay Select - Phase Delay and Clock Phase Register - D6

LFP --> 5 Group Phase Delay operate at low frequency PWM (LFP)
HFP --> 5 Group Phase Delay operate at high frequency PWM (HFP) 
*/
enum HLF {
    LFP = 0,
    HFP = 1,
};

/*
Clock Phase Select - Phase Delay and Clock Phase Register - D5:D0
*/
enum PSn {
    DISABLE = 0,
    ENABLE = 1
};





class LEDController {
public: 
private:
};


#endif // LED_CONTROLLER_HPP