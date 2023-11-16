#include <stdio.h>
#include "hardware/ultrasonic.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

// Global variables declaration for ultrasonic measurements
uint64_t width;
int successful_pulse;
absolute_time_t start_time;
absolute_time_t end_time;
uint64_t final_result = -1;  // Initialize final result with an error value

// Function to initialize ultrasonic sensor GPIO pins
void initUltrasonic(void *params) {
    gpio_init(ULTRASONIC_ECHO);
    gpio_init(ULTRASONIC_TRIG);

    gpio_set_dir(ULTRASONIC_ECHO, GPIO_IN);
    gpio_set_dir(ULTRASONIC_TRIG, GPIO_OUT);
}

// Function to trigger the ultrasonic sensor
void pulseUltrasonic(void *params) {
    gpio_put(ULTRASONIC_TRIG, 1);
    vTaskDelay(1);  // Add a brief delay for the ultrasonic pulse
    gpio_put(ULTRASONIC_TRIG, 0);
}

// Function to measure distance using the ultrasonic sensor
uint64_t getDistanceUltrasonic(void *params) {
    width = 0;
    successful_pulse = 0;

    while (gpio_get(ULTRASONIC_ECHO) == 0) {
        tight_loop_contents();
    }

    start_time = get_absolute_time();

    while (gpio_get(ULTRASONIC_ECHO) == 1) {
        width++;

        if (width > ULTRASONIC_TIMEOUT) {
            successful_pulse = 0;
        }
        else {
            successful_pulse = 1;
        }
    }

    if (successful_pulse == 1) {
        end_time = get_absolute_time();
        final_result = absolute_time_diff_us(start_time, end_time) / 29 / 2; // Calculate distance based on time difference
        return final_result;
    } else {
        return -1;  // Return error value if measurement was unsuccessful
    }
}

// Callback function for ultrasonic sensor GPIO interrupts
void gpio_callback_ultrasonic(uint gpio, uint32_t events) {
    getDistanceUltrasonic(NULL);
}

/*** end of file ***/