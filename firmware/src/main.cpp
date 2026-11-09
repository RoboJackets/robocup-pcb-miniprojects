#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include "LEDController.hpp"

// SPI Defines, need to check ports on RP2040
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// I2C defines, need to check ports on RP2040
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

LEDController led_controller = LEDController(I2C_PORT, LEDControllerAddress::AD_GND);

void core1_entry() {
    // I2C Initialisation. Using it at 1MHz
    i2c_init(I2C_PORT, 1000*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    //Configure the LED Controller
    led_controller.setConfigs(
        PowerControl(
            SSDMode::DISABLE,
            PMSResolution::R_10BIT, 
            B14Mode::DISABLE, 
            HFPS::F_32kHZ, 
            RGBMode::M_8RGB
        ), 
        PhaseDelay(
            PDE::ENABLE, 
            HLF::LFP,
            PSn::DISABLE,
            PSn::DISABLE,
            PSn::DISABLE,
            PSn::DISABLE,
            PSn::DISABLE,
            PSn::DISABLE
        ), 
        SpreadSpectrum());

    //testing out basic functions
    led_controller.setHFP(1, 1 * 1024/8);
    led_controller.setHFP(2, 2 * 1024/8);
    led_controller.setHFP(3, 3 * 1024/8);
    led_controller.setHFP(4, 4 * 1024/8);
    led_controller.setHFP(5, 5 * 1024/8);
    led_controller.setHFP(6, 6 * 1024/8);
    led_controller.setHFP(7, 7 * 1024/8);
    led_controller.setHFP(8, 8 * 1024/8);

    led_controller.setLFP(1, 255);
    led_controller.setLFP(2, 0);
    led_controller.setLFP(3, 0);
    led_controller.setLFP(4, 0);
    led_controller.setLFP(5, 255);
    led_controller.setLFP(6, 0);
    led_controller.setLFP(7, 0);
    led_controller.setLFP(8, 0);
    led_controller.setLFP(9, 255);
    led_controller.setLFP(10, 255);
    led_controller.setLFP(11, 255);
    led_controller.setLFP(12, 0);
    led_controller.setLFP(13, 255);
    led_controller.setLFP(14, 0);
    led_controller.setLFP(15, 255);
    led_controller.setLFP(16, 0);
    led_controller.setLFP(17, 255);
    led_controller.setLFP(18, 255);
    led_controller.setLFP(19, 255);
    led_controller.setLFP(20, 255);
    led_controller.setLFP(21, 255);

    led_controller.setLFP(22, 128);
    led_controller.setLFP(23, 0);
    led_controller.setLFP(24, 255);

    led_controller.update();
}


int main()
{
    stdio_init_all();
    //multicore example, core 0 for spi, core 1 for i2c, at least that is the goal
    multicore_launch_core1(core1_entry);

    // SPI initialisation at 1 MHz
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    //placeholder while loop
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
