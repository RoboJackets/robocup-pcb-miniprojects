#pragma once

#include "pico/types.h"
#include "pico/mutex.h" 

#define R_RX 16
#define R_TX 15
#define R_CLK 14 
#define R_SS 17
#define S_TX 14
#define S_RX 2
#define S_CLK 0
#define S_SS 0  

// Frequency in MHz
#define FREQ 1

// MagX (high) on byte 14 (119..112)
// MagX (low) on byte 15 (127..120)
// MagY (h) on byte 16 (135..128)
// MagY (l) on byte 17 (143..136)
// MagZ (h) on byte 18 (151..144)
// MagZ (l) on byte 19 (159..152)

// Byte length of buffer that we have to read/write
#define CLUB_BUF_LEN 56 

/**
 * @brief Output buffer directly modified in main by calling the sensor using the mutex
 */
uint8_t out_buf[CLUB_BUF_LEN];
uint8_t in_buf[CLUB_BUF_LEN]; 
/**
 * @brief Data buffer for Hall Effect data
 */
uint8_t data_buf[6]; 

mutex_t output_mutex; 


/**
 * @brief Initalize the SPI protocol
 * */
void spi_init();