#pragma once

#include "pico/stdlib.h" // includes GPIO, time, and stdio
#include "hardware/spi.h"
#include "spi.hpp"
#include <mutex>
#include <array>

/** RP2040 GPIO Pins
 * | GPIO   | F1       | F2        | F3       | F4     | F5  | F6   | F7   | F8            | F9            |
 * |--------|----------|-----------|----------|--------|-----|------|------|---------------|---------------|
 * | 0      | SPI0 RX  | UART0 TX  | I2C0 SDA | PWM0 A | SIO | PIO0 | PIO1 |               | USB OVCUR DET |
 * | 1      | SPI0 CSn | UART0 RX  | I2C0 SCL | PWM0 B | SIO | PIO0 | PIO1 |               | USB VBUS DET  |
 * | 2      | SPI0 SCK | UART0 CTS | I2C1 SDA | PWM1 A | SIO | PIO0 | PIO1 |               | USB VBUS EN   |
 * | 3      | SPI0 TX  | UART0 RTS | I2C1 SCL | PWM1 B | SIO | PIO0 | PIO1 |               | USB OVCUR DET |
 * | 4      | SPI0 RX  | UART1 TX  | I2C0 SDA | PWM2 A | SIO | PIO0 | PIO1 |               | USB VBUS DET  |
 * | 5      | SPI0 CSn | UART1 RX  | I2C0 SCL | PWM2 B | SIO | PIO0 | PIO1 |               | USB VBUS EN   |
 * | 6      | SPI0 SCK | UART1 CTS | I2C1 SDA | PWM3 A | SIO | PIO0 | PIO1 |               | USB OVCUR DET |
 * | 7      | SPI0 TX  | UART1 RTS | I2C1 SCL | PWM3 B | SIO | PIO0 | PIO1 |               | USB VBUS DET  |
 * | 8      | SPI1 RX  | UART1 TX  | I2C0 SDA | PWM4 A | SIO | PIO0 | PIO1 |               | USB VBUS EN   |
 * | 9      | SPI1 CSn | UART1 RX  | I2C0 SCL | PWM4 B | SIO | PIO0 | PIO1 |               | USB OVCUR DET |
 * | 10     | SPI1 SCK | UART1 CTS | I2C1 SDA | PWM5 A | SIO | PIO0 | PIO1 |               | USB VBUS DET  |
 * | 11     | SPI1 TX  | UART1 RTS | I2C1 SCL | PWM5 B | SIO | PIO0 | PIO1 |               | USB VBUS EN   |
 * | 12     | SPI1 RX  | UART0 TX  | I2C0 SDA | PWM6 A | SIO | PIO0 | PIO1 |               | USB OVCUR DET |
 * | 13     | SPI1 CSn | UART0 RX  | I2C0 SCL | PWM6 B | SIO | PIO0 | PIO1 |               | USB VBUS DET  |
 * | 14     | SPI1 SCK | UART0 CTS | I2C1 SDA | PWM7 A | SIO | PIO0 | PIO1 |               | USB VBUS EN   |
 * | 15     | SPI1 TX  | UART0 RTS | I2C1 SCL | PWM7 B | SIO | PIO0 | PIO1 |               | USB OVCUR DET |
 * | 16     | SPI0 RX  | UART0 TX  | I2C0 SDA | PWM0 A | SIO | PIO0 | PIO1 |               | USB VBUS DET  |
 * | 17     | SPI0 CSn | UART0 RX  | I2C0 SCL | PWM0 B | SIO | PIO0 | PIO1 |               | USB VBUS EN   |
 * | 18     | SPI0 SCK | UART0 CTS | I2C1 SDA | PWM1 A | SIO | PIO0 | PIO1 |               | USB OVCUR DET |
 * | 19     | SPI0 TX  | UART0 RTS | I2C1 SCL | PWM1 B | SIO | PIO0 | PIO1 |               | USB VBUS DET  |
 * | 20     | SPI0 RX  | UART1 TX  | I2C0 SDA | PWM2 A | SIO | PIO0 | PIO1 | CLOCK GPIN0   | USB VBUS EN   |
 * | 21     | SPI0 CSn | UART1 RX  | I2C0 SCL | PWM2 B | SIO | PIO0 | PIO1 | CLOCK GPOUT0  | USB OVCUR DET |
 * | 22     | SPI0 SCK | UART1 CTS | I2C1 SDA | PWM3 A | SIO | PIO0 | PIO1 | CLOCK GPIN1   | USB VBUS DET  |
 * | 23     | SPI0 TX  | UART1 RTS | I2C1 SCL | PWM3 B | SIO | PIO0 | PIO1 | CLOCK GPOUT1  | USB VBUS EN   |
 * | 24     | SPI1 RX  | UART1 TX  | I2C0 SDA | PWM4 A | SIO | PIO0 | PIO1 | CLOCK GPOUT2  | USB OVCUR DET |
 * | 25     | SPI1 CSn | UART1 RX  | I2C0 SCL | PWM4 B | SIO | PIO0 | PIO1 | CLOCK GPOUT3  | USB VBUS DET  |
 * | 26     | SPI1 SCK | UART1 CTS | I2C1 SDA | PWM5 A | SIO | PIO0 | PIO1 |               | USB VBUS EN   |
 * | 27     | SPI1 TX  | UART1 RTS | I2C1 SCL | PWM5 B | SIO | PIO0 | PIO1 |               | USB OVCUR DET |
 * | 28     | SPI1 RX  | UART0 TX  | I2C0 SDA | PWM6 A | SIO | PIO0 | PIO1 |               | USB VBUS DET  |
 * | 29     | SPI1 CSn | UART0 RX  | I2C0 SCL | PWM6 B | SIO | PIO0 | PIO1 |               | USB VBUS EN   | 
 */

// pins for master (receives packet)
// <TODO> check pcb design for actual pin numbers
#define SPI_MASTER spi0
constexpr uint8_t PIN_MISO_MASTER = 0;
constexpr uint8_t PIN_CS_MASTER   = 1;
constexpr uint8_t PIN_SCK_MASTER  = 2;
constexpr uint8_t PIN_MOSI_MASTER = 3;

// pins for slave (transmits packet)
#define SPI_SLAVE spi1
constexpr uint PIN_MOSI_SLAVE = 8;
constexpr uint PIN_CS_SLAVE   = 9;
constexpr uint PIN_SCK_SLAVE  = 10;
constexpr uint PIN_MISO_SLAVE = 11;

constexpr uin8_t PACKET_LENGTH = 56;

enum Error {
    NO_ERROR,
    GEN_ERROR,
    BAD_CHECKSUM,
    BAD_TRANSFER_LENGTH,
    NO_DMA_CHANNEL
};


class LEDSPI {
    public:
        /** @brief Create a new instance of the SPI driver.
         * 
         * @param packet Address of packet containing data for the chain.
         * @param p_mutex Address of shared mutex to guard the packet.
         */
        LEDSPI(std::array<uint8_t, PACKET_LENGTH>& packet, std::mutex& p_mutex);

        /** @brief Sets up pins and basic functionality
         */
        void init();

        /** @brief Requests data from a connected slave device.
         *
         * @return Returns success or error type.
         */
        Error read_blocking();

        /** @brief Writes current data to the transmit FIFO (max 8 bytes), does not end until there is enough space for full transfer.
         *
         * @return Returns success or error type.
         */
        Error write_blocking();

        /** @brief Calls read_blocking() and write_blocking().
         * 
         * @return Returns success or error type.
        */
        Error read_write_blocking();


        // <TODO>
        Error read_nonblocking();
        Error write_nonblocking();
        Error read_write_nonblocking();

    private:
        std::array<uint8_t, PACKET_LENGTH>& packet_;
        std::mutex& packet_mutex_;
        std::array<uint8_t, PACKET_LENGTH> buffer_{};
};
