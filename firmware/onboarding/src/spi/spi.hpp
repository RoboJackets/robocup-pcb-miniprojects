#pragma once

#include "pico/types.h"

// In bus
#define SPI0_BUS 0 

// Out
#define SPI1_BUS 1

// MagX (high) on byte 14 (119..112)
// MagX (low) on byte 15 (127..120)
// MagY (h) on byte 16 (135..128)
// MagY (l) on byte 17 (143..136)
// MagZ (h) on byte 18 (151..144)
// MagZ (l) on byte 19 (159..152)

// Byte length of buffer that we have to read/write
#define CLUB_BUF_LEN 0x38

class SPI {
    size_t buf_length;
    const uint8_t* out_buf; 
    uint8_t* in_buf;

    uint8_t RX, TX, CLK, SS;     
    
    SPI(uint8_t RX, uint8_t TX, uint8_t CLK, uint8_t SS, size_t buf_length); 


    /**
     * @brief Initalize the SPI protocol
     * 
     * @param RX Receiving pin (MISO)
     * @param TX Transmitting pin (MOSI)
     * @param CLK Clock pin
     * @param SS Slave select pin
     * @param prod Is code running in production (default to true)
     * @return int; 0 for completion, 1 for failure 
     */
    int init(bool prod=true);
    
    /**
     * @brief Test function **DO NOT USE IN PROD** 
     * 
     * @return * void 
     */
    void run_test();

    /**
     * @brief Set the interrupt on a specific pin with a flag
     * 
     * @param PIN A uint8_t pin number
     * @param FLAG A uint32_t: 
     * - @ref GPIO_IRQ_LEVEL_LOW -- Trigger when GPIO is logical 0 
     * - @ref GPIO_IRQ_LEVEL_HIGH -- Trigger when GPIO is logical 1
     * - @ref GPIO__IRQ_EDGE_FALL -- Trigger on falling edge
     * - @ref GPIO_IRQ_EDGE_RISE -- Trigger on rising edge
    */ 
    void set_interrupt(uint8_t PIN, uint32_t FLAG); 

    static void run(SPI self); 

    void transmit();

    void receive(); 
};
