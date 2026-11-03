#include "board.hpp"

SENSORS getSensor(uint8_t num) {
    switch(num) {
        case 0: {
            return SENSORS::HALL;
            break;
        }
        case 1: {
            return SENSORS::COLOR;
            break;
        }
        case 2: {
            return SENSORS::TEMP;
            break;
        }
        case 3: {
            return SENSORS::IMU;
            break;
        }
        case 4: {
            return SENSORS::GPIO;
            break;
        }
        case 5: {
            return SENSORS::LED;
            break;
        }
    }
}

void cycleStates(bool forward) {
    current_sensor += (current_sensor == 5) ? -5 : ((forward) ? 1 : -1);
    switch(current_sensor) {
        case 0: {
            gpio_put(LED_1, 0);
            gpio_put(LED_2, 0);
            gpio_put(LED_2, 0);
            break;
        }
        case 1: {
            gpio_put(LED_1, 1);
            gpio_put(LED_2, 0);
            gpio_put(LED_2, 0);
            break;
        }
        case 2: {
            gpio_put(LED_1, 0);
            gpio_put(LED_2, 1);
            gpio_put(LED_2, 0);
            break;
        }
        case 3: {
            gpio_put(LED_1, 1);
            gpio_put(LED_2, 1);
            gpio_put(LED_2, 0);
            break;
        }
        case 4: {
            gpio_put(LED_1, 0);
            gpio_put(LED_2, 0);
            gpio_put(LED_2, 1);
            break;
        }
        case 5: {
            gpio_put(LED_1, 1);
            gpio_put(LED_2, 0);
            gpio_put(LED_2, 1);
            break;
        }
    }
}

void set_interrupts() {
    gpio_set_irq_enabled_with_callback(BUTTON_1, GPIO_IRQ_EDGE_FALL, true, &interrupt_attach); 
    gpio_set_irq_enabled_with_callback(BUTTON_2, GPIO_IRQ_EDGE_FALL, true, &interrupt_attach); 
}

void init_buttons() {
    gpio_init(BUTTON_1);
    gpio_init(BUTTON_2);
    gpio_set_dir(BUTTON_1, 0);
    gpio_set_dir(BUTTON_2, 0);
    gpio_init(LED_1);
    gpio_init(LED_2);
    gpio_set_dir(LED_1, 1);
    gpio_set_dir(LED_2, 1);
}

void interrupt_attach(uint gpio, uint32_t events) {
    if (gpio == BUTTON_1 && (events & GPIO_IRQ_EDGE_FALL)) {
        cycleStates(true); 
    }
    else if (gpio == BUTTON_2 && (events & GPIO_IRQ_EDGE_FALL)) {
        cycleStates(false);
    }
}
