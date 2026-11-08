#pragma once
// GPIO assignments, schematic Rev 11.7.1
// Owner: shared (agree before changing)

// ---- I2C: one bus per device (only 2 controllers -> switch with gpio_set_function) ----
#define PIN_SDA_IMU      0   // I2C0
#define PIN_SCL_IMU      1   // I2C0
#define PIN_SDA_TEMP     2   // I2C1
#define PIN_SCL_TEMP     3   // I2C1
#define PIN_SDA_HALL     4   // I2C0
#define PIN_SCL_HALL     5   // I2C0
#define PIN_SDA_GPIO     22  // I2C1 (GPIO expander)
#define PIN_SCL_GPIO     23  // I2C1
#define PIN_SDA_COLOR    24  // I2C0
#define PIN_SCL_COLOR    25  // I2C0
#define PIN_SDA_LED      26  // I2C1 (LED driver)
#define PIN_SCL_LED      27  // I2C1

#define I2C_BAUD_HZ      (400 * 1000)

#define PIN_LED_SDB      6   // IS31FL3254 enable, high = on (100k pull-down)

// ---- Debug LEDs (active-low: write 0 to turn on) ----
#define PIN_LED_D39      8
#define PIN_LED_D40      9
#define PIN_LED_D41      10

// ---- Buttons (active-low: read 0 when pressed, RC debounced) ----
#define PIN_BUTTON_1     16  // S4
#define PIN_BUTTON_2     17  // S5

// ---- SPI OUT: SPI1, we are MASTER (to next board, J7) ----
#define PIN_SPI_OUT_MISO 12  // SPI1 RX  (we receive)
#define PIN_SPI_OUT_CSN  13  // SPI1 CSn
#define PIN_SPI_OUT_SCK  14  // SPI1 SCK
#define PIN_SPI_OUT_MOSI 15  // SPI1 TX  (we send)

// ---- SPI IN: SPI0, we are SLAVE (from previous board, J9) ----
#define PIN_SPI_IN_SCK   18  // SPI0 SCK
#define PIN_SPI_IN_MISO  19  // SPI0 TX  (we send back to master)
#define PIN_SPI_IN_MOSI  20  // SPI0 RX  (we receive)
#define PIN_SPI_IN_CSN   21  // SPI0 CSn

#define SPI_BAUD_HZ      (1000 * 1000)   // 1 MHz, mode 3 (CPOL = 1, CPHA = 1)

// Unused: GPIO 7, 11, 28, 29
