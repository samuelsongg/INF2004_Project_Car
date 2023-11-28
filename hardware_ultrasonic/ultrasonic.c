/** 
 * @file ultrasonic.c
 *
 * @brief Implements functions for interfacing with an ultrasonic sensor.
 * 
 * This module contains the implementation of functions necessary for
 * initializing and using an ultrasonic sensor. It includes sensor initialization,
 * triggering pulse, distance measurement, and handling GPIO interrupts.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2023 Team 61. All rights reserved.
 */
#include <stdio.h>
#include "hardware/ultrasonic.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

// Global variables to store measurement data
uint64_t width;
int successful_pulse;
absolute_time_t start_time;
absolute_time_t end_time;
uint ultrasonic_distance = -1;  // Default value indicating error or no measurement

/**
 * Retrieves the last measured ultrasonic distance.
 * 
 * @param params Optional parameters (unused in this function).
 * @return The measured ultrasonic distance.
 */
uint getUltrasonicFinalResult(void *params) {
    return ultrasonic_distance;
}

/**
 * Initializes the GPIO pins used by the ultrasonic sensor.
 *
 * @param params Optional parameters (unused in this function).
 */
void initUltrasonic(void *params) {
    gpio_init(ULTRASONIC_ECHO);
    gpio_init(ULTRASONIC_TRIG);

    gpio_set_dir(ULTRASONIC_ECHO, GPIO_IN);
    gpio_set_dir(ULTRASONIC_TRIG, GPIO_OUT);
}

/**
 * Triggers the ultrasonic sensor to emit a pulse.
 *
 * @param params Optional parameters (unused in this function).
 */
void pulseUltrasonic(void *params) {
    gpio_put(ULTRASONIC_TRIG, 1);
    vTaskDelay(1);  // Delay to ensure ultrasonic pulse is sent
    gpio_put(ULTRASONIC_TRIG, 0);
}

/**
 * Measures the distance using the ultrasonic sensor.
 *
 * @param params Optional parameters (unused in this function).
 * @return The measured distance or -1 if the measurement failed.
 */
uint64_t getDistanceUltrasonic(void *params) {
    width = 0;
    successful_pulse = 0;

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
        ultrasonic_distance = absolute_time_diff_us(start_time, end_time) / 29 / 2; // Calculate distance based on time difference

        return ultrasonic_distance;
    }
    else {
        return -1;  // Indicates measurement failure
    }
}

/**
 * GPIO callback function for ultrasonic sensor interrupts.
 *
 * @param gpio GPIO number.
 * @param events Type of event that triggered the interrupt.
 */
void gpio_callback_ultrasonic(uint gpio, uint32_t events) {
    getDistanceUltrasonic(NULL);
}

/*** End of file ***/
