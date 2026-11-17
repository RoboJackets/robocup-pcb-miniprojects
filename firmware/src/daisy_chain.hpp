/**
 * @file daisy_chain.hpp
 * @brief SPI daisy chain: receives the 56-byte packet from the previous board
 *        and forwards it to the next board.
 *
 * Hardware roles
 * --------------
 *   SPI0 (IN port)  : we are the slave; the previous board drives the clock and chip select.
 *   SPI1 (OUT port) : we are the master; we drive the clock and chip select for the next board.
 *   Format: 1 MHz, mode 3 (CPOL = 1, CPHA = 1), 8 bits, MSB first.
 *
 * What one frame is
 * -----------------
 * The bytes sent while chip select (CS) is low form one frame; only exactly 56 bytes
 * count as a valid packet.
 * TODO: confirm this frame definition with the mentor.
 *
 * Why mode 3
 * ----------
 * When the RP2040's SPI (ARM PL022) is a slave with CPHA = 0, CS must go high after
 * every byte. With CPHA = 1, CS can stay low for all 56 bytes, so
 * "one CS pulse = one packet" works.
 *
 * How receiving works
 * -------------------
 * 1. DMA moves each byte from SPI0 into a buffer by itself, without the CPU.
 * 2. A rising edge on CS (end of frame) triggers an interrupt that checks for exactly 56 bytes.
 * 3. A valid packet is stored and flagged as new; an invalid one is dropped.
 *    Then the DMA is re-armed for the next frame.
 * So even if we power up in the middle of a frame, we re-align on the next one.
 *
 * Only core 0 may call this module (the interrupt is registered on core 0).
 */
#pragma once

#include <cstdint>

#include "packet.hpp"

namespace daisy_chain {

struct Stats {
    uint32_t received;     // good packets
    uint32_t bad_frames;   // wrong length, dropped
    uint32_t overwritten;  // new packet came in before the old one was read
    uint32_t sent;
};

/// SPI0 slave + SPI1 master + DMA + CS interrupt. Call once on core 0.
void init();

/// Copies the newest packet into `out` if there is one. Doesn't wait.
bool receive(packet::Packet &out);

/// Sends a packet out SPI1. Blocks until done (~0.5 ms at 1 MHz).
void send(const packet::Packet &pkt);

Stats stats();

}  // namespace daisy_chain
