#ifndef SPIBUS
#define SPIBUS

#include "pico/stdlib.h" // includes GPIO, time, and stdio
#include "pico/mutex.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "hardware/dma.h"


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


// pins for slave, receives packet from previous device
#define SPI_SLAVE spi0
constexpr uint8_t PIN_SCK_SLAVE  = 18;
constexpr uint8_t PIN_MISO_SLAVE = 19;
constexpr uint8_t PIN_MOSI_SLAVE = 20;
constexpr uint8_t PIN_CS_SLAVE   = 21;

// pins for master, transmits packet to next device
#define SPI_MASTER spi1
constexpr uint8_t PIN_MISO_MASTER = 12;
constexpr uint8_t PIN_CS_MASTER   = 13;
constexpr uint8_t PIN_SCK_MASTER  = 14;
constexpr uint8_t PIN_MOSI_MASTER = 15;

constexpr size_t PACKET_LENGTH = 56;

enum class SPIError {
    NO_ERROR,
    GEN_ERROR,
    BAD_CHECKSUM,
    BAD_TRANSFER_LENGTH,
    NO_DMA_CHANNEL,
};


class SPIBus {

    /* Overall notes:
    Only the blocking or only the nonblocking functions should be used.
    Nonblocking uses direct memory access and is better in theory but is more error-prone and is probably harder to debug.
    Blocking is basically just the Pico SDK read and write with some wrapper code for data safety and other things.
    Currently allowing master RX FIFO and slave TX FIFO overruns because they are unused.
    */ 

    public:
        /** @brief Creates a new instance of the SPI bus.
         * 
         * @param packet Pointer to the packet containing data for the chain.
         * @param p_mutex Reference to a shared mutex to guard the packet, used by reading and writing functions.
         */
        SPIBus(uint8_t packet[], mutex_t& p_mutex);

        /** @brief Sets up pins and basic functionality.
         * 
         * @return Returns success or error type.
         */
        SPIError init();

        /** @brief Sets up DMA (direct memory access) for nonblocking reading and writing.
         * 
         * @return Returns success or error type.
        */
        SPIError init_dma();

        /** @brief Resets RX DMA channel, should be used with errors or timeouts.
         * 
         * @param init Reinitializes the channel if true, only resets if false.
         * 
         * @return Returns success or error type.
        */
        SPIError reset_dma_rx(bool init);

        /** @brief Resets RTX DMA channel, should be used with errors or timeouts.
         * 
         * @param init Reinitializes the channel if true, only resets if false.
         * 
         * @return Returns success or error type.
        */
        SPIError reset_dma_tx(bool init);

        /** @brief Reads data from PIN_MOSI_SLAVE to buffer_, polls until transfer completes.
         */
        SPIError read_blocking();

        /** @brief Writes data from packet_ to PIN_MOSI_MASTER and sends it, polls until transfer completes.
         *
         * @return Returns success or error type.
         */
        SPIError write_blocking();

        /** @brief Reads data from PIN_MOSI_SLAVE to buffer_, waits without polling until transfer completes.
         * 
         * @return Returns success or error type.
        */ 
        SPIError read_nonblocking();

        /** @brief Writes data from packet_ to PIN_MOSI_MASTER and sends it, waits without polling until transfer completes.
         * 
         * @return Returns success or error type.
        */ 
        SPIError write_nonblocking();

        /** @brief Updates packet_ from buffer_, should be used after reading.
         * 
         * @return Returns success or error type.
         * 
         * @note Can be used with both blocking and nonblocking functions.
        */
        SPIError update_packet();

        /** @brief Reads and sets new_data_, automatically called after a GPIO interrupt.
         * 
         * @param gpio The gpio pin that causes the interrupt.
         * @param events The level or edge of the signal that causes the interrupt.
         * 
         * @note Must be void and have uint gpio and uint32_t events parameters to work with the Pico SDK.
         * @note Must be static to be a class member function.
         * @note Updating and writing should be handled in the main loop to prevent mutex deadlocks.
        */ 
        static void handle_cs_fall(uint gpio, uint32_t events);

        /** @brief Reinitializes a DMA channel when its transfer completes. 
         * 
         * @note Must be void have no parameters to work with hardware.
         * @note Must be static to be a class member function.
        */ 
        static void handle_dma();

        /** @brief Sets PIN_CS_MASTER high after it is done writing, automatically called after an alarm.
         * 
         * @param id The alarm_id as returned when the alarm was added.
         * @param user_data The user data passed when the alarm was added.
         * 
         * @note Must return int64_t and have alarm_id_t id and void *user_data parameters to work with the Pico SDK.
         * @note Must be static to be a class member function.
         */
        static int64_t handle_alarm(alarm_id_t id, void *user_data);

        /** @brief Sets whether a good checksum is required to update the packet data, initially false.
        */
        void set_checksum_required(bool required);

        /** @brief Sets the read function that handles CS low interrupts.
         * 
         * @param blocking Sets read_blocking() if true, read_nonblocking() if false.
        */
        void set_callback_read(bool blocking);

        /** @brief Sets whether the RX DMA channel restarts automatically or needs to be restarted in main.
         * 
         * @param automatic Reinitializes dma_rx_ in handle_dma() if true, does not if false.
        */
        void set_auto_read_nonblocking(bool automatic);

        
    private:
        uint8_t* packet_;
        mutex_t& p_mutex_;
        uint8_t buffer_[PACKET_LENGTH] = {};

        int8_t dma_tx_ = -1;
        int8_t dma_rx_ = -1;
        dma_channel_config config_tx_;
        dma_channel_config config_rx_;

        SPIError (SPIBus::*callback_read_)() = &SPIBus::read_blocking;
        static inline SPIBus* bus_ = nullptr;

        alarm_id_t alarm_;

        volatile bool new_data_ = false; // should be cleared and handled by main when true
        bool checksum_required_ = false;
        bool auto_read_nonblocking_ = true;
};

#endif //SPIBUS