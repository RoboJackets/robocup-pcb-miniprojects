#include "pico/stdlib.h"

#define BUTTON_1 0
#define BUTTON_2 0 
#define LED_1 0 
#define LED_2 0 
#define LED_3 0 

enum class SENSORS {
    HALL, // 0
    COLOR, // 1
    TEMP, // 2
    IMU, // 3
    GPIO, // 4 
    LED // 5
};

uint8_t current_sensor = 0;

SENSORS getSensor(uint8_t num);
void interrupt_attach(uint gpio, uint32_t events); 
void set_interrupts();
void cycleStates(bool forward); 

