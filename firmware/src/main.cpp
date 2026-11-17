/**
 * @file main.cpp
 * @brief Fulgora sensor-suite firmware entry point.
 *
 * Normal mode:
 *   Packet from the previous board -> insert the DIP switch value, set Board ID bit 4 -> forward.
 *   Follows the README rule: only write after an input trigger, but always output the most recent data.
 *
 * Test-source mode (CMake option FULGORA_SPI_TEST_SOURCE=ON):
 *   Sends a generated test packet out of SPI1 every 200 ms and prints packets received on SPI0.
 *   Used for testing without the PCB: one Pico in loopback, or two Picos in a ring.
 *
 * TODO: move sensor reading to core 1.
 */
#include <cstdio>

#include "board_config.hpp"
#include "daisy_chain.hpp"
#include "hardware/gpio.h"
#include "i2c_bus.hpp"
#include "packet.hpp"
#include "pico/stdlib.h"
#include "tca9534.hpp"

namespace {

constexpr uint kStatusLedPin = PICO_DEFAULT_LED_PIN;  // GPIO25 on Pico, GPIO8 on our board

constexpr uint32_t kDipReadPeriodMs = 50;
constexpr uint32_t kStatusPeriodMs = 1000;
constexpr uint32_t kConfigPeriodMs = 10000;
#ifdef FULGORA_SPI_TEST_SOURCE
constexpr uint32_t kTestSendPeriodMs = 200;
#endif

#ifdef FULGORA_BOARD
constexpr const char *kTarget = "Fulgora PCB";
#else
constexpr const char *kTarget = "Raspberry Pi Pico (dev)";
#endif

constexpr board::Device kExpander = board::Device::GpioExpander;

// latest DIP reading, gets copied into every packet we forward
uint8_t g_dip = 0;
bool g_dip_valid = false;

void printConfig() {
    printf("\n=== %s%s ===\n", kTarget,
#ifdef FULGORA_SPI_TEST_SOURCE
           " [SPI TEST SOURCE]"
#else
           ""
#endif
    );
    printf("SPI IN : spi%u  RX=%u CSn=%u SCK=%u TX=%u (slave)\n", board::kSpiIn.instance, board::kSpiIn.rx,
           board::kSpiIn.csn, board::kSpiIn.sck, board::kSpiIn.tx);
    printf("SPI OUT: spi%u  RX=%u CSn=%u SCK=%u TX=%u (master)\n", board::kSpiOut.instance, board::kSpiOut.rx,
           board::kSpiOut.csn, board::kSpiOut.sck, board::kSpiOut.tx);
    printf("SPI    : %lu Hz, mode 3, %u-byte packets, our Board ID bit = %u\n",
           static_cast<unsigned long>(board::kSpiBaudHz), static_cast<unsigned>(packet::kSize),
           static_cast<unsigned>(packet::BoardId::GpioExpander));
}

// prints OK/-- for each I2C device, handy for checking wiring
void scanDevices() {
    printf("I2C scan:\n");
    for (uint8_t i = 0; i < board::kDeviceCount; ++i) {
        const auto device = static_cast<board::Device>(i);
        const auto &d = board::device(device);
        printf("  I2C%u SDA=%2u SCL=%2u 0x%02X %-18s %s\n", d.instance, d.sda, d.scl, d.address, d.name,
               i2c_bus::probe(device) ? "OK" : "--");
    }
}

// keeps retrying init() until the chip shows up, so late wiring / hot-plug works
void readDip(tca9534::Tca9534 &expander) {
    static bool ready = false;

    i2c_bus::BusLock lock(kExpander);

    if (!ready) {
        const tca9534::Status s = expander.init();
        ready = (s == tca9534::Status::Ok);
        if (!ready) {
            g_dip_valid = false;
            if (s == tca9534::Status::Timeout) i2c_bus::recover(kExpander);  // bus is probably stuck
            return;
        }
    }

    uint8_t raw = 0;
    uint8_t changed = 0;
    if (expander.readChanges(raw, changed) == tca9534::Status::Ok) {
        g_dip = raw;
        g_dip_valid = true;
        if (changed) printf("DIP changed: raw=0x%02X (mask 0x%02X)\n", raw, changed);
    } else {
        ready = false;
        g_dip_valid = false;
    }
}

#ifdef FULGORA_SPI_TEST_SOURCE
void printPacket(const char *label, const packet::Packet &p) {
    printf("%s ids=0x%02X gpio=0x%02X temp=%d\n", label, p.boardIds(), p.gpioInputs(), p.temperature());
}
#else

// normal mode: add our data to the incoming packet and pass it on
void handleIncoming(packet::Packet &pkt) {
    if (g_dip_valid) {
        // TODO: ask mentor if this should be raw (ON = 0) or inverted
        pkt.setGpioInputs(g_dip);
        pkt.setBoardId(packet::BoardId::GpioExpander);
    }
    // still forward even if the DIP read failed (just without our bit) so the chain keeps working
    daisy_chain::send(pkt);
}
#endif

bool due(absolute_time_t &deadline, uint32_t period_ms) {
    if (absolute_time_diff_us(get_absolute_time(), deadline) > 0) return false;
    deadline = make_timeout_time_ms(period_ms);
    return true;
}

absolute_time_t earliest(absolute_time_t a, absolute_time_t b) {
    return absolute_time_diff_us(a, b) > 0 ? a : b;
}

}  // namespace

int main() {
    stdio_init_all();

    gpio_init(kStatusLedPin);
    gpio_set_dir(kStatusLedPin, GPIO_OUT);

    i2c_bus::init();
    daisy_chain::init();

    const auto &dev = board::device(kExpander);
    tca9534::Tca9534 expander(board::i2cInstance(dev), dev.address);

    absolute_time_t next_dip = get_absolute_time();
    absolute_time_t next_status = get_absolute_time();
    absolute_time_t next_config = get_absolute_time();
#ifdef FULGORA_SPI_TEST_SOURCE
    absolute_time_t next_test = get_absolute_time();
    uint16_t test_counter = 0;
#endif

    bool led_on = false;
    packet::Packet pkt;

    while (true) {
        if (due(next_config, kConfigPeriodMs)) {
            printConfig();
            scanDevices();
        }

        if (due(next_dip, kDipReadPeriodMs)) readDip(expander);

#ifdef FULGORA_SPI_TEST_SOURCE
        // pretend to be the debug input board. counter goes in the temp field so packets are easy to tell apart
        if (due(next_test, kTestSendPeriodMs)) {
            packet::Packet test;
            test.setBoardId(packet::BoardId::DebugInput);
            test.setTemperature(static_cast<int16_t>(test_counter++));
            daisy_chain::send(test);
        }
        if (daisy_chain::receive(pkt)) printPacket("RX", pkt);
#else
        if (daisy_chain::receive(pkt)) handleIncoming(pkt);
#endif

        if (due(next_status, kStatusPeriodMs)) {
            const daisy_chain::Stats s = daisy_chain::stats();
            printf("SPI rx=%lu bad=%lu overwritten=%lu tx=%lu | DIP %s0x%02X\n",
                   static_cast<unsigned long>(s.received), static_cast<unsigned long>(s.bad_frames),
                   static_cast<unsigned long>(s.overwritten), static_cast<unsigned long>(s.sent),
                   g_dip_valid ? "" : "(no chip) ", g_dip);
            led_on = !led_on;
            gpio_put(kStatusLedPin, led_on);
        }

        // sleep until the next timer is due, the CS interrupt wakes us early if a packet comes in
        absolute_time_t wake = earliest(next_dip, earliest(next_status, next_config));
#ifdef FULGORA_SPI_TEST_SOURCE
        wake = earliest(wake, next_test);
#endif
        best_effort_wfe_or_timeout(wake);
    }
}
