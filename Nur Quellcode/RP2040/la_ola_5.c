#include <stdio.h>

#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#define LED1_PIN 2
#define LED2_PIN 3
#define LED3_PIN 4
#define LED4_PIN 5
#define LED5_PIN 6
#define LED6_PIN 7

#define DEFAULT_BRIGHTNESS 4096
#define MAXIMUM_BRIGHTNESS 65535

uint brightnesses[] = {
    MAXIMUM_BRIGHTNESS << 0,
    DEFAULT_BRIGHTNESS << 3,
    DEFAULT_BRIGHTNESS << 2,
    DEFAULT_BRIGHTNESS << 1,
    DEFAULT_BRIGHTNESS << 0,
    DEFAULT_BRIGHTNESS << 1,
    DEFAULT_BRIGHTNESS << 2,
    DEFAULT_BRIGHTNESS << 3};

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

// Offset from GPIO pin number to index in array
#define LED_PIN_OFFSET 1
// Set the brightness according to the brightnesses in the array
void set_brightness() {
    for (uint led = LED1_PIN; led <= LED6_PIN; led++) pwm_set_gpio_level(led, brightnesses[led - LED_PIN_OFFSET]);
}

//Rotate the array to create la ola animation
void rotate_brightnesses() {
    // Last index in the array
    uint max_index = sizeof(brightnesses)/sizeof(brightnesses[0]) - 1;
    // The last element becomes the first
    uint last_element = brightnesses[max_index];
    //Move all values left by one index
    for (uint index = max_index; index > 0; index--) {
        brightnesses[index] = brightnesses[index - 1];
    }
    // Put the previos last element into first position
    brightnesses[0] = last_element;
}

// Callback to advance the wave by one
bool laola_timer_callback(struct repeating_timer *t) {
    set_brightness();
    rotate_brightnesses();
    return true;
}

int main() {
    setup_pwm();

    // Repeating timer to animate la ola in approx 1 second per rotation
    struct repeating_timer la_ola;
    add_repeating_timer_ms(-125, &laola_timer_callback, NULL, &la_ola);

    while (1) tight_loop_contents();
}