/** 
 * @file ultrasonic.h
 *
 * @brief Provides functions for interfacing with an ultrasonic sensor.
 * 
 * This module contains the definitions and function declarations necessary for
 * initializing and using an ultrasonic sensor. Functions include sensor initialization,
 * triggering pulse, distance measurement, and GPIO interrupt handling.
 */
#ifndef _ULTRASONIC_H
#define _ULTRASONIC_H

#include "hardware/ultrasonic.h"
#include "hardware/gpio.h"

// Definitions for ultrasonic sensor pins and timeout value.
#define ULTRASONIC_ECHO 10
#define ULTRASONIC_TRIG 11
#define ULTRASONIC_TIMEOUT 26100

/**
 * Initializes the ultrasonic sensor.
 *
 * @param params Optional parameters (unused in this function).
 */
void initUltrasonic(void *params);

/**
 * Triggers a pulse from the ultrasonic sensor.
 *
 * @param params Optional parameters (unused in this function).
 */
void pulseUltrasonic(void *params);

/**
 * Measures the distance using the ultrasonic sensor.
 *
 * @param params Optional parameters (unused in this function).
 * @return Measured distance as a 64-bit unsigned integer.
 */
uint64_t getDistanceUltrasonic(void *params);

/**
 * Handles GPIO callbacks for the ultrasonic sensor.
 *
 * @param gpio GPIO number associated with the callback.
 * @param events GPIO event that triggered the callback.
 */
void gpio_callback_ultrasonic(uint gpio, uint32_t events);

/**
 * Retrieves the last measured distance from the ultrasonic sensor.
 *
 * @param params Optional parameters (unused in this function).
 * @return The last measured distance.
 */
uint getUltrasonicFinalResult(void *params);

#endif /* _ULTRASONIC_H */

/*** End of file ***/
