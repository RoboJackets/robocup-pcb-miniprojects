#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include "LEDController.hpp"

// SPI Defines
//TODO: need to check ports on RP2040
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// I2C defines
//TODO: need to check ports on RP2040
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

//Interface defines, buttons and LEDs
//TODO: check pin numbers
#define BUTTON_1 10
#define BUTTON_2 11
#define LED_1 12
#define LED_2 13
#define LED_3 14

enum Devices : uint8_t {
    NONE, 
    IMU,
    TEMPERATURE,
    HALLEFFECT,
    GPIOEXPANDER,
    COLOR,
    LEDCONTROLLER
};

static Devices curDevice = Devices::NONE;

//device scroll function for interface
//TODO: check if device scroll interrupt conflicts with central callback function
void device_scroll(uint gpio, uint32_t events) {
    if (gpio == BUTTON_1) {
        curDevice = static_cast<Devices>((((uint8_t) curDevice) + 1) % 7);
    }

    if (gpio == BUTTON_2) {
        curDevice = static_cast<Devices>((((uint8_t) curDevice) - 1) % 7);
    }
}

//TODO: Run I2C device on core 1
void core1_entry() {
    //placeholder
    while (true) {
        printf("Hello from Core 1!\n");
        sleep_ms(1000);
    }
}

//TODO: Run SPI on core 0
int main()
{
    stdio_init_all();

    // SPI initialisation at 1 MHz
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    // I2C Initialisation. Using it at 1MHz
    i2c_init(I2C_PORT, 1000*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    //Interface button and LEDs intializations
    gpio_init(LED_1);
    gpio_set_dir(LED_1, GPIO_OUT);
    gpio_init(LED_2);
    gpio_set_dir(LED_2, GPIO_OUT);
    gpio_init(LED_3);
    gpio_set_dir(LED_3, GPIO_OUT);

    gpio_init(BUTTON_1);
    gpio_set_dir(BUTTON_1, GPIO_IN);
    gpio_pull_up(BUTTON_1);

    gpio_init(BUTTON_2);
    gpio_set_dir(BUTTON_2, GPIO_IN);
    gpio_pull_up(BUTTON_2);

    // Launch Core 1
    multicore_launch_core1(core1_entry);

    //TODO: main code, possible while loop (polling)

    //SPI needs to recieve packet from previous board
    //Based on I2C device selected, needs to do operations to or from the packet
    //SPI needs to send packet to next board
}
