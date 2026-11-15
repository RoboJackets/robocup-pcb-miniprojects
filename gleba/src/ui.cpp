#include "ui.hpp"

#include "pico/stdlib.h"
#include "pins.hpp"

// ---- internal helpers ----

// NEW: what each button looked like at the previous ui_update() call.
static bool prev_btn1 = false;
static bool prev_btn2 = false;

// True once per press (debounced). Buttons are active-low.
static bool button_pressed(uint pin) {
    return !gpio_get(pin);
}

// LEDs are active-low: on = drive 0.
static void led_write(uint pin, bool on) {
    gpio_put(pin, !on);
}

// ---- public ----

void ui_init(void) {
    gpio_init(PIN_BUTTON_1);
    gpio_init(PIN_BUTTON_2);
    gpio_init(PIN_LED_D39);
    gpio_init(PIN_LED_D40);
    gpio_init(PIN_LED_D41);

    gpio_set_dir(PIN_BUTTON_1, GPIO_IN);
    gpio_set_dir(PIN_BUTTON_2, GPIO_IN);
    gpio_set_dir(PIN_LED_D39, GPIO_OUT);
    gpio_set_dir(PIN_LED_D40, GPIO_OUT);
    gpio_set_dir(PIN_LED_D41, GPIO_OUT);
}

void ui_update(void) {
    if (button_pressed(PIN_BUTTON_1)) {
        shared_set_selected(static_cast<Device>(static_cast<int>(shared_get_selected()) - 1));
        // Change current i2c connection
        ui_show_selection();
    }

    if (button_pressed(PIN_BUTTON_2)) {
        shared_set_selected(static_cast<Device>(static_cast<int>(shared_get_selected()) + 1));
        // Change current i2c connection
        ui_show_selection();
    }
}

void ui_show_selection(void) {
    int num = static_cast<int>(shared_get_selected());
    led_write(PIN_LED_D39, (bool)(num / 4));
    led_write(PIN_LED_D40, (bool)((num % 4) / 2));
    led_write(PIN_LED_D41, (bool)(num % 2));
}
