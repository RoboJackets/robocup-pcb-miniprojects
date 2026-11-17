/**
 * @file daisy_chain.cpp
 * @brief SPI daisy chain implementation.
 */
#include "daisy_chain.hpp"

#include <cstring>

#include "board_config.hpp"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/time.h"

namespace daisy_chain {

namespace {

constexpr size_t kPacketSize = packet::kSize;

spi_inst_t *const g_spi_in = board::spiInstance(board::kSpiIn);
spi_inst_t *const g_spi_out = board::spiInstance(board::kSpiOut);

uint8_t g_rx_buf[kPacketSize];  // DMA writes here

// last good packet, written in the IRQ and read from the main loop
uint8_t g_latest[kPacketSize];
volatile bool g_has_new = false;

volatile Stats g_stats = {0, 0, 0, 0};

int g_rx_dma = -1;

void drainRxFifo() {
    while (spi_is_readable(g_spi_in)) {
        (void)spi_get_hw(g_spi_in)->dr;
    }
}

// TODO: spec doesn't say what goes back on MISO yet, just send zeros for now
void fillTxFifo() {
    while (spi_is_writable(g_spi_in)) {
        spi_get_hw(g_spi_in)->dr = 0;
    }
}

void armRxDma() {
    dma_channel_config c = dma_channel_get_default_config(static_cast<uint>(g_rx_dma));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, false);  // always the SPI data reg
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, spi_get_dreq(g_spi_in, false));  // pace it with SPI RX

    dma_channel_configure(static_cast<uint>(g_rx_dma), &c, g_rx_buf, &spi_get_hw(g_spi_in)->dr, kPacketSize,
                          true);
}

// CS went high = end of a frame
void onChipSelectRise() {
    const uint pin = board::kSpiIn.csn;
    if (!(gpio_get_irq_event_mask(pin) & GPIO_IRQ_EDGE_RISE)) return;  // shared IRQ, not ours
    gpio_acknowledge_irq(pin, GPIO_IRQ_EDGE_RISE);

    const uint ch = static_cast<uint>(g_rx_dma);
    const uint32_t remaining = dma_channel_hw_addr(ch)->transfer_count;
    dma_channel_abort(ch);

    // good only if exactly 56 bytes came in (DMA done and nothing extra left in the FIFO)
    const bool exact_length = (remaining == 0) && !spi_is_readable(g_spi_in);

    if (exact_length) {
        if (g_has_new) g_stats.overwritten = g_stats.overwritten + 1;
        std::memcpy(g_latest, g_rx_buf, kPacketSize);
        g_has_new = true;
        g_stats.received = g_stats.received + 1;
    } else {
        // e.g. we booted halfway through a frame, next one will line up
        g_stats.bad_frames = g_stats.bad_frames + 1;
    }

    drainRxFifo();
    fillTxFifo();
    armRxDma();

    __sev();  // wake the main loop if it's in wfe
}

void setupSlave() {
    // baud doesn't really matter for a slave, the master's clock is used
    spi_init(g_spi_in, board::kSpiBaudHz);
    spi_set_format(g_spi_in, board::kSpiDataBits, board::kSpiCpol, board::kSpiCpha, SPI_MSB_FIRST);
    spi_set_slave(g_spi_in, true);

    gpio_set_function(board::kSpiIn.rx, GPIO_FUNC_SPI);
    gpio_set_function(board::kSpiIn.csn, GPIO_FUNC_SPI);
    gpio_set_function(board::kSpiIn.sck, GPIO_FUNC_SPI);
    gpio_set_function(board::kSpiIn.tx, GPIO_FUNC_SPI);

    // keep CS/SCK high when nothing is plugged in so we don't see fake frames
    gpio_pull_up(board::kSpiIn.csn);
    gpio_pull_up(board::kSpiIn.sck);

    g_rx_dma = dma_claim_unused_channel(true);
    drainRxFifo();
    fillTxFifo();
    armRxDma();

    // GPIO IRQs still fire even though the pin's function is SPI
    gpio_acknowledge_irq(board::kSpiIn.csn, GPIO_IRQ_EDGE_RISE);
    gpio_add_raw_irq_handler(board::kSpiIn.csn, onChipSelectRise);
    gpio_set_irq_enabled(board::kSpiIn.csn, GPIO_IRQ_EDGE_RISE, true);
    irq_set_enabled(IO_IRQ_BANK0, true);
}

void setupMaster() {
    spi_init(g_spi_out, board::kSpiBaudHz);
    spi_set_format(g_spi_out, board::kSpiDataBits, board::kSpiCpol, board::kSpiCpha, SPI_MSB_FIRST);

    gpio_set_function(board::kSpiOut.rx, GPIO_FUNC_SPI);
    gpio_set_function(board::kSpiOut.sck, GPIO_FUNC_SPI);
    gpio_set_function(board::kSpiOut.tx, GPIO_FUNC_SPI);

    // CS by hand. hardware CS can pop high if the FIFO runs dry for a moment,
    // which would split the packet
    gpio_init(board::kSpiOut.csn);
    gpio_put(board::kSpiOut.csn, true);
    gpio_set_dir(board::kSpiOut.csn, GPIO_OUT);
}

}  // namespace

void init() {
    setupMaster();
    setupSlave();
}

bool receive(packet::Packet &out) {
    if (!g_has_new) return false;

    // IRQs off during the copy so a new packet can't land halfway through
    const uint32_t saved = save_and_disable_interrupts();
    std::memcpy(out.bytes().data(), g_latest, kPacketSize);
    g_has_new = false;
    restore_interrupts(saved);
    return true;
}

void send(const packet::Packet &pkt) {
    constexpr uint32_t kCsSetupUs = 2;

    gpio_put(board::kSpiOut.csn, false);
    busy_wait_us_32(kCsSetupUs);

    // waits for the last bit to go out and clears the RX FIFO for us
    spi_write_blocking(g_spi_out, pkt.bytes().data(), kPacketSize);

    busy_wait_us_32(kCsSetupUs);
    gpio_put(board::kSpiOut.csn, true);

    g_stats.sent = g_stats.sent + 1;
}

Stats stats() {
    const uint32_t saved = save_and_disable_interrupts();
    const Stats s{g_stats.received, g_stats.bad_frames, g_stats.overwritten, g_stats.sent};
    restore_interrupts(saved);
    return s;
}

}  // namespace daisy_chain
