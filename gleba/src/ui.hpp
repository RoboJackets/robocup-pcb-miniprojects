#pragma once
// Buttons + 3 debug LEDs: choose one of the six I2C devices and show which
// one is selected (binary code 1-6 on LEDs D39/D40/D41). Called from core 1.
// Owner: Yueqi

#include "shared.hpp"

// Set up button inputs and LED outputs (all LEDs off).
void ui_init(void);

// Check the buttons; if the selection changed, store it with
// shared_set_selected() and update the LEDs. Call often from core 1's loop.
void ui_update(void);

// Show a device number on the 3 LEDs (DEV_IMU -> 1, ..., DEV_LED -> 6).
void ui_show_selection(Device dev);
