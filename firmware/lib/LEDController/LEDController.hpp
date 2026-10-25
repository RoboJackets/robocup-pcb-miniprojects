//LED Controller Driver
// DataSheet for Controller : https://www.lumissil.com/assets/pdf/core/IS31FL3254_DS.pdf
// DataSheet for LEDS : https://downloads.cree-led.com/files/ds/h/HB-CLMVC-FKA.pdf

#ifndef LED_CONTROLLER_HPP
#define LED_CONTROLLER_HPP

/* A2:A1 --> AD, A0 --> Read/Write
 AD connected to GND, AD = 00;
 AD connected to VCC, AD = 11;
 AD connected to SCL, AD = 01;
 AD connected to SDA, AD = 10; */
#define LED_CONTROLLER_ADDRESS 0b0110_0000

//Power Control Register -- R/W
#define CONTROL_REG 0x00

//High Frequency PWM(HFP) Duty Register -- R/W
//OUT [24:1] high frequency PWM register byte
#define HFP_1 0x01
#define HFP_1_end 0x0C
#define HFP_2 0x19
#define HFP_2_end 0x2A
#define HFP_3 0x31
#define HFP_3_end 0x3C
#define HFP_4 0x43
#define HFP_4_end 0x48

//Low Frequency PWM(LFP) Duty Register -- R/W
//OUT [24:1] low frequency PWM register byte
#define LFP_1 0x49
#define LFP_1_end 0x4E
#define LFP_2 0x55
#define LFP_2_end 0x5D
#define LFP_3 0x61
#define LFP_3_end 0x66
#define LFP_4 0x6A
#define LFP_4_end 0x6C

//Update the HFP & LFP data -- W
#define UPDATE_REG 0x6D

//Global Current Control Registers -- R/W
#define GLOBAL_CURCONTROL_R 0x6E
#define GLOBAL_CURCONTROL_G 0x6F
#define GLOBAL_CURCONTROL_B 0x70

//Phase Delay and Clock Phase -- R/W
#define PHASE_REG 0x71

//Spread Spectrum -- R/W
#define SPREAD_SPECTRUM_REG 0x72

//Reset -- W
#define RESET_REG 0x7F





#endif // LED_CONTROLLER_HPP