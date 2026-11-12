// LED Controller Driver for IS31FL3254 using picosdk
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
enum LEDControllerAddress : uint8_t {
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

If enabled, the device will enter software shutdown mode with all current sources switched off, leading to LEDs being off.

Registers remain accessible.
*/
enum class SSDMode : uint8_t {
    ENABLE = 0,
    DISABLE = 1
};

/** 
 * High PWM Frequency Resolution
 * 
 * If 8Bit, HFPS will decide the internal oscillator clock frequency and the PWM output PWM frequency.
 * 
 * If 10Bit, the HFP is 32kHz, LFP is 127Hz, not matter how HFPS is set.
*/
enum PMSResolution : uint8_t {
    R_8BIT = 0,
    R_10BIT = 1
};

/*
10+4-bit Mode

If disabled, the working mode is controlled by PMS setting.
If enabled, will enable 10+4-bit with 4-bit dithering mode.
*/
enum class B14Mode : uint8_t {
    DISABLE = 0,
    ENABLE = 1
};

/**
 * High Frequency PWM Frequency Selection
 * 
 * Defaults to 32kHZ if 10-bit enabled
 * 
 * HFPS also selects the Low Frequency PWM frequency and the OSC frequency.
 * 
 * Table 4 from IS31FL3254 DataSheet
 * 
 * +-----+------+-----------+----------+-----------+
 * | PMS | HFPS | OSC (MHZ) | LFP (HZ) | HFP (kHZ) |
 * +-----+------+-----------+----------+-----------+
 * |  1  |  xx  |     32    |    127   |    32     |
 * +-----+------+-----------+----------+-----------+
 * |     |  00  |     8     |    127   |    32     |
 * |     +------+-----------+----------+-----------+
 * |  0  |  01  |     16    |    254   |    64     |
 * |     +------+-----------+----------+-----------+
 * |     |  1x  |     32    |    508   |    128    |
 * +-----+------+-----------+----------+-----------+
*/
enum HFPS : uint8_t {
    F_32kHZ = 0,
    F_64kHZ = 1,
    F_128kHZ = 2
};


/*
RGB Register Mode Select 

24CH --> each of the 24 channels are controlled by it’s own HFP PWM register.
8RGB --> pairs of 3 channels controlled by same HFP PWM register for RGB combinations
*/
enum RGBMode : uint8_t {
    M_24CH = 0,
    M_8RGB = 1
};

#pragma pack(push, 1)
/*
    Power Control Configurations

    Struct holds all the settings for the Power Control Register.
*/
struct PowerControl {

    //Default constructor sets all the configs to default values
    PowerControl();

    /**
     * Constructor with all the config parameters
     * 
     * @param ssd Software Shutdown Mode
     * @param pms PWM Resolution
     * @param b14 10+4-bit Mode
     * @param hfps High Frequency PWM Frequency Selection
     * @param rgb RGB Register Mode Select
     */
    PowerControl(SSDMode ssd, PMSResolution pms, B14Mode b14, HFPS hfps, RGBMode rgb);

    private:

    /**
     * Converts all the configs into one byte to be written into the register
     * 
     * @return the byte
     */
    uint8_t toByte();

    SSDMode software_shutdown : 1;
    PMSResolution resolution : 1;
    B14Mode b14_enable : 1;
    uint8_t : 1; // reserved
    HFPS hfp_select : 2;
    RGBMode channel_mode : 1;
    uint8_t : 1; // reserved

    friend class LEDController;
};
#pragma pack(pop)

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

HFP Registers have two registers always in consecutive order
Here are the lower bit addresses, the higher bit addresses are always +1
*/

//24 Channel Mode

#define HFP1 0x01
#define HFP2 0x03
#define HFP3 0x05
#define HFP4 0x07
#define HFP5 0x09
#define HFP6 0x0B
#define HFP7 0x19
#define HFP8 0x1B
#define HFP9 0x1D
#define HFP10 0x1F
#define HFP11 0x21
#define HFP12 0x23
#define HFP13 0x25
#define HFP14 0x27
#define HFP15 0x29
#define HFP16 0x31
#define HFP17 0x33
#define HFP18 0x35
#define HFP19 0x37
#define HFP20 0x39
#define HFP21 0x3B
#define HFP22 0x43
#define HFP23 0x45
#define HFP24 0x47

//8 RGB Mode

#define HFP_RGB1 0x01
#define HFP_RGB2 0x03
#define HFP_RGB3 0x09
#define HFP_RGB4 0x0B
#define HFP_RGB5 0x0D
#define HFP_RGB6 0x11
#define HFP_RGB7 0x13
#define HFP_RGB8 0x17

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

#define GCCR 0x6E
#define GCCG 0x6F
#define GCCB 0x70

/*
Global Current Control Registers -- R/W

Modulates all channels DC current in 256 steps.
Each color controls the current of the respective color channels. (Usually every third channel)
*/
enum GCC {
    RED,
    GREEN,
    BLUE
};

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

If enabled, these groups will operate PWM at different phases to reduce the voltage ripple of LED power supply.

Specifically, after group 3 starts operating PWM, group 4 will start operating PWM after a 1/(6 x freq) period, and so on.

Every group is delayed by 1/(6 x freq) period from the previous group, except for group 2 which is delayed by 2/(6 x freq) period from group 1.
*/
enum class PDE : uint8_t {
    DISABLE = 0,
    ENABLE = 1,
};

/*
Frequency Group Phase Delay Select

Choose whether the phase delay scheme is applied to the high frequency PWM or low frequency PWM.

It's recommended to apply the phase delay scheme to the low frequency PWM, since the high frequency PWM ripple isn't as effective as the low frequency PWM ripple.

LFP --> Low Frequency PWM
HFP --> High Frequency PWM
*/
enum HLF : uint8_t {
    LFP = 0,
    HFP = 1,
};

/*
Clock Phase Select

IS31FL3254 features a clock phase function.

The 24 outputs are separated into 5 groups:
 - Group 1: 1-->6
 - Group 2: 7-->12
 - Group 3: 13-->15
 - Group 4: 16-->21
 - Group 5: 22-->24

If group [n]'s clock phase is enabled, the channels in that group will alternate between Clock Phase 1 and Clock Phase 2.
Clock Phase 1's falling edge happens at the same time as Clock Phase 2's rising edge to reduce the voltage ripple of LED power supply.

*NOTE: Revision B of the IS31FL3254 data sheet reduces the number of groups from 6 to 5, yet this is not explicitly stated in the 71h register map. Ignore the 6th group.

*/
enum PSn : uint8_t {
    DISABLE = 0,
    ENABLE = 1
};

#pragma pack(push, 1)
/**
 * Phase Delay and Clock Phase Configurations
 * 
 * Holds all the settings for the Phase Delay and Clock Phase Register
 */
struct PhaseDelay {
    //Default constructor sets all the configs to default values
    PhaseDelay();

    /**
     * Constructor with all the config parameters
     * 
     * @param enable Phase Delay Enable
     * @param high_freq Group Phase Delay Select
     * @param phase_1 Clock Phase Select for Group 1
     * @param phase_2 Clock Phase Select for Group 2
     * @param phase_3 Clock Phase Select for Group 3
     * @param phase_4 Clock Phase Select for Group 4
     * @param phase_5 Clock Phase Select for Group 5
     * @param phase_6 Clock Phase Select for Group 6
     */
    PhaseDelay(PDE enable, HLF high_freq, PSn phase_1, PSn phase_2, PSn phase_3, PSn phase_4, PSn phase_5, PSn phase_6);

    private:
    /** 
     * Converts the configurations into a single byte to be written into the register
     * 
     * @return the byte
    */
    uint8_t toByte();

    PSn phase_1 : 1;
    PSn phase_2 : 1;
    PSn phase_3 : 1;
    PSn phase_4 : 1;
    PSn phase_5 : 1;
    PSn phase_6 : 1;
    HLF high_freq : 1;
    PDE enable : 1;

    friend class LEDController;
};
#pragma pack(pop)

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
    Spread Spectrum Synchronization

    If enabled, the spread spectrum function will be synchronized with the PWM cycle to prevent visible LED flickering
*/
enum class SSY : uint8_t {
    DISABLE = 0,
    ENABLE = 1
};

/*
    Spread Spectrum Enable
    
    If enabled, the spread spectrum function will be enabled to reduce EMI.
*/
enum class SSP : uint8_t {
    DISABLE = 0,
    ENABLE = 1
};

/*
    Spread Spectrum Cycle Time

    Sets the modulation cycle time: 500ms, 125ms, 1.4ms, or 0.35ms
*/
enum CLT : uint8_t {
    t_500ms = 0b00,
    t_125ms = 0b01,
    t_1_4ms = 0b10,
    t_0_35ms = 0b11,
};

/*
    Spread Spectrum Range 

    Selects the frequency modulation deviation range (±5%, ±13%, ±19%, or ±25%).
*/
enum RNG : uint8_t {
    R_05 = 0b00,
    R_13 = 0b01,
    R_19 = 0b10,
    R_25 = 0b11,
};

#pragma pack(push, 1)
/**
 * Spread Spectrum Configurations
 * 
 * Struct to hold all the settings for the Spread Spectrum Register
 */
struct SpreadSpectrum {

    //Default constructor sets all the configs to default values
    SpreadSpectrum();

    /**
     * Constructor with all the config parameters
     * 
     * @param synch_enable Spread Spectrum Synch Enable
     * @param spectrum_enable Spread Spectrum Enable
     * @param range Spread Spectrum Range
     * @param cycle_time Spread Spectrum Cycle Time
     */
    SpreadSpectrum(SSY synch_enable, SSP spectrum_enable, RNG range, CLT cycle_time);

    private:
    /** 
     * Converts the configurations into a single byte to be written into the register
     * 
     * @return the byte
    */
    uint8_t toByte();

    CLT cycle_time : 2;
    RNG range : 2;
    SSP spectrum_enable : 1;
    SSY synch_enable : 1;
    uint8_t : 2; //reserved

    friend class LEDController;
};
#pragma pack(pop)

/*
Reset Register - W

write of "0000 0000" to this register will reset all regsiters to their default values
*/ 

#define RESET_REG 0x7F

/**
 * LED Controller Driver for IS31FL3254 using picosdk
 * 
 * This class provides an interface for controlling the IS31FL3254 LED controller.
 * 
 * Basic use:
 * 
 * Initialize the LEDController with the desired I2C bus and address
 * LEDController led_controller(i2c, LEDControllerAddress::AD_GND);
 * 
 * Initialize the PowerControl, PhaseDelay, and SpreadSpectrum structs with the desired configurations
 * led_controller.setConfigs(power_control, phase_delay, spread_spectrum);
 * 
 * Set the Low Frequency PWM value of the desired LED channel (this is the basic brightness control)
 * led_controller.setLFP(channel, value);
 * 
 * Refer to https://www.lumissil.com/assets/pdf/core/IS31FL3254_DS.pdf for more information on other features.
 */
class LEDController {
public: 
    /**
     * @brief Create a new instance of the LED Controller Driver with default configs.
     * 
     * @param i2c the i2c bus being used
     * @param address the address of the LED Controller
     */
    LEDController(i2c_inst_t* i2c, LEDControllerAddress address);

    /**
     * @brief Configures the LED Controller with the desired Power Control, Phase Delay, and Spread Spectrum settings
     * 
     * @param power_control Power Control struct
     * @param phase_delay Phase Delay struct
     * @param spread_spectrum Spread Spectrum struct
     * 
     * @returns true if operation is successful
     */
    bool setConfigs(PowerControl power_control, PhaseDelay phase_delay, SpreadSpectrum spread_spectrum);

    /**
     * @brief Sets the Low Frequency PWM value of the desired LED channel
     * 
     * If value is beyond the resolution, will set to maximum possible value
     * 
     * @param channel Desired LED channel, 1-24
     * @param value 0 to 255
     * 
     * @return true if operation is successful
     */
    bool setLFP(uint8_t channel, uint8_t value);

    /**
     * @brief Sets the High Frequency PWM value of the desired LED channel
     * 
     * If value is beyond the resolution, will set to maximum possible value
     * 
     * @param channel Desired LED channel, 1-24 for 24CH mode, 1-8 for 8RGB mode
     * @param value 0 to 255 (8-bit), 1024 (10-bit), or 16384 (10+4-bit)
     * 
     * @return true if operation is successful
     */
    bool setHFP(uint8_t channel, uint16_t value);

    /**
     * @brief Sets the Frequency Mode of the desired LED channel
     * 
     * @param channel Desired LED channel, 1-24 for 24CH mode, 1-8 for 8RGB mode
     * @param fms The FMS mode 
     * 
     * @return true if operation is successful
     */
    bool setFMS(uint8_t channel, FMSMode fms);

    /**
     * @brief Sets the global current of the red, green, or blue channels
     * 
     * The maximum current is set by the physical resistor, the GCC only modulates the current in 256 steps.
     * 
     * @param channel Desired LED channel
     * @param value current stepping 0-255
     * @return true if operation is successful
     */
    bool setGCC(GCC channel, uint8_t value);

    /**
     * @brief updates the Low and High Frequency PWM channels
     * 
     * @return true if operation is successful
     */
    bool update();

    /**
     * @brief resets all registers to their default value
     * 
     * @return true if operation is successful
     */
    bool reset();

private:
    /**
     * @brief Write a number of bytes starting at a given register address
     * 
     * @param reg The register address on the LED Controller to start writing to
     * @param length The number of bytes to write
     * @param data The data to write to the LED Controller
     * @return True if the operation was successful
     */
    bool write_bytes(uint8_t reg, uint8_t length, uint8_t *data);

     /**
     * @brief write a byte to a given register address 
     * 
     * @param reg The register address on the LED Controller to write to
     * @param data The byte data to write to the address
     * @return True if the operation was successful
     */
    bool write_byte(uint8_t reg, uint8_t data);

    /**
     * @brief Read a number of bytes starting from a register address
     * 
     * @param reg The start address
     * @param length The number of bytes to read
     * @param data The buffer to store the output into
     * @return True if the operation was successful
     */
    bool read_bytes(uint8_t reg, uint8_t length, uint8_t *data);

    /**
     * @brief Read from a given register
     * 
     * @param reg The address to read from
     * @param data The output data 
     * @return True if the operation was successful
     */
    bool read_byte(uint8_t reg, uint8_t *data);

    uint8_t address_;
    PowerControl pcontrol_;
    PhaseDelay pdelay_;
    SpreadSpectrum ss_;
    i2c_inst_t *i2c_;
};

#endif // LED_CONTROLLER_HPP