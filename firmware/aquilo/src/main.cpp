#include "TempSensor.hpp"
#include <cstdint>
#include <hardware/gpio.h>
#include <hardware/regs/i2c.h>
#include <hardware/spi.h>
#include <hardware/structs/io_bank0.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <pico/thread_local.h>

// TODO: define these later
#define SPI_IN_RX ((uint8_t)0)
#define SPI_IN_SS ((uint8_t)0)
#define SPI_IN_SCLK ((uint8_t)0)

#define SPI_OUT_TX ((uint8_t)0)
#define SPI_OUT_SS ((uint8_t)0)
#define SPI_OUT_SCLK ((uint8_t)0)


void SPI_setup()
{
    spi_init(spi0, 1000 * 1000);
    spi_init(spi1, 1000 * 1000);

    // set spi0 to slave (Take from last board)
    spi_set_slave(spi0, true);
    // set spi1 to master (Send to next board)
    spi_set_slave(spi1, false);

    // set spi format
    spi_set_format(spi0, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_set_format(spi1, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    
    // set spi in pins
    gpio_set_function(SPI_IN_RX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_IN_SS, GPIO_FUNC_SPI);
    gpio_set_function(SPI_IN_SCLK, GPIO_FUNC_SPI);

    // set spi out pins
    gpio_init(SPI_OUT_SS);
    gpio_put(SPI_OUT_SS, 1);    // init to not selecting slave
    gpio_set_dir(SPI_OUT_SS, GPIO_OUT);
    gpio_set_function(SPI_OUT_TX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_OUT_SCLK, GPIO_FUNC_SPI);
}

// TODO: I2C PIN SETUP
#define I2C_SDA_TEMP 0
#define I2C_SCL_TEMP 0
void I2C_setup()
{
    gpio_set_function(I2C_SDA_TEMP, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_TEMP, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_TEMP);
    gpio_pull_up(I2C_SCL_TEMP);
}

// TODO: need to add multiprocessing
int main()
{
    // init
    I2C_setup();
    SPI_setup();
    TempSensor temp_sensor(&i2c0_inst, 0x48);

    while (true)
    {
        // read incoming packet (SPI)
        uint8_t spi_packet[56];
        spi_read_blocking(spi0, 0, spi_packet, sizeof(spi_packet));

        // add reading to packet
        uint8_t reading[2];
        if (temp_sensor.readTemp(reading))
        {
            spi_packet[12] = reading[0];
            spi_packet[13] = reading[1];
            // set board checksum bit (using bit 2 for temp sensor)
            // is this a "valid read bit" or a participation bit?
            spi_packet[55] |= (1u << 2);
        }
        else
        {
            spi_packet[12] = 255;
            spi_packet[13] = 255;
            spi_packet[55] &= ~(1u << 2);
        }

        // forward pacxket to next board (SPI)
        gpio_put(SPI_OUT_SS, 0);
        spi_write_blocking(spi1, spi_packet, sizeof(spi_packet));
        gpio_put(SPI_OUT_SS, 1);
        sleep_ms(10);
    }
}
