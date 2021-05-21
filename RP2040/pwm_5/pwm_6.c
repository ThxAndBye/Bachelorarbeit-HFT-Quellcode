#include <stdio.h>

#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#define LED1_PIN 2
#define LED2_PIN 3
#define LED3_PIN 4
#define LED4_PIN 5
#define LED5_PIN 6
#define LED6_PIN 7
#define BUTTON_LEFT 18
#define BUTTON_MIDDLE 19
#define BUTTON_RIGHT 20

#define DEFAULT_BRIGHTNESS 256
#define MAXIMUM_BRIGHTNESS 65535

#define DEBOUNCE_MS 50

uint brightness_1 = DEFAULT_BRIGHTNESS;
uint brightness_2 = DEFAULT_BRIGHTNESS;

bool is_debounceing = false;

// Setup pins with default PWM config
void setup_pwm() {
    // Get some sensible defaults for the slice configuration.
    pwm_config config = pwm_get_default_config();

    // Each 2 pins will allocate 1 PWM slice
    for (uint led_pin = LED1_PIN; led_pin <= LED6_PIN; led_pin++) {
        // Set pin function as PWM
        gpio_set_function(led_pin, GPIO_FUNC_PWM);
        // Get slice number of current pin
        uint slice_num = pwm_gpio_to_slice_num(led_pin);
        // Load the configuration into our PWM slice, and set it running.
        pwm_init(slice_num, &config, true);
    }
}

// Callback to reset debouncing flag
int64_t debounce_alarm_callback(alarm_id_t id, void *user_data) {
    is_debounceing = false;
    return 0;
}

// Method to debounce switch input
bool debounce() {
    if (!is_debounceing) {
        add_alarm_in_ms(DEBOUNCE_MS, &debounce_alarm_callback, NULL, false);
        is_debounceing = true;
        return false;
    }
    return true;
}

// Handle GPIO interrupts
void gpio_callback(uint gpio, uint32_t events) {
    // Return if debouncing is active.
    if (debounce()) return;

    switch (gpio) {
        // Increase bightness of first LED
        case BUTTON_MIDDLE:
            brightness_1 <<= 1;
            //Maximum brightness reached
            if (brightness_1 > MAXIMUM_BRIGHTNESS) brightness_1 = MAXIMUM_BRIGHTNESS;
            pwm_set_gpio_level(LED1_PIN, brightness_1);
            break;
        // Increase bightness of last LED
        case BUTTON_RIGHT:
            brightness_2 <<= 1;
            //Maximum brightness reached
            if (brightness_2 > MAXIMUM_BRIGHTNESS) brightness_2 = MAXIMUM_BRIGHTNESS;
            pwm_set_gpio_level(LED6_PIN, brightness_2);
            break;
        // Reset brightness
        default:
            brightness_1 = DEFAULT_BRIGHTNESS;
            brightness_2 = DEFAULT_BRIGHTNESS;
            pwm_set_gpio_level(LED1_PIN, brightness_1);
            pwm_set_gpio_level(LED6_PIN, brightness_2);
            break;
    }
}

int main() {
    setup_pwm();

    // Attach the interrupts to the buttons on rising edge
    gpio_set_irq_enabled_with_callback(BUTTON_LEFT, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_MIDDLE, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_RIGHT, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    while (1) tight_loop_contents();
}