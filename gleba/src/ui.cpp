#include "ui.hpp"

#include "pico/stdlib.h"
#include "pins.hpp"

// ---- internal helpers ----

// True once per press (debounced). Buttons are active-low.
static bool button_pressed(uint pin) {
    // TODO
    return false;
}

// LEDs are active-low: on = drive 0.
static void led_write(uint pin, bool on) {
    gpio_put(pin, !on);
}

// ---- public ----

void ui_init(void) {
    // TODO: gpio_init + direction for PIN_BUTTON_1/2 (in) and PIN_LED_D39/40/41 (out, off)
}

void ui_update(void) {
    // TODO: button 1 -> next device, button 2 -> previous device (wrap around DEV_COUNT)
    //       shared_set_selected(dev); ui_show_selection(dev);
}

void ui_show_selection(Device dev) {
    // TODO: code = dev + 1; led_write(D39, bit0); led_write(D40, bit1); led_write(D41, bit2);
}
