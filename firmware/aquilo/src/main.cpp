#include "TempSensor.hpp"
#include <cstdint>
#include <hardware/spi.h>
#include <pico/time.h>

int main()
{
    // init
    TempSensor temp_sensor(&i2c0_inst, 0x48);
    // set spi0 to slave
    spi_set_slave(spi0, true);
    // set spi1 to master
    spi_set_slave(spi1, false);

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
            spi_packet[13] = reading[0];
        }
        else
        {
            spi_packet[12] = 255;
            spi_packet[13] = 255;
        }

        // forward pacxket to next board (SPI)
        spi_write_blocking(spi1, spi_packet, sizeof(spi_packet));
        sleep_ms(10);
    }
}
