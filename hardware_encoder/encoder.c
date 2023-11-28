/** @file encoder.c
 *
 * @brief This module handles the operations related to wheel encoders in a robotic car.
 *        It includes functions for calculating the speed and distance traveled by each wheel.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/encoder.h"

// Global variables to store measurement data for the left wheel
volatile uint32_t leftNotchCount = 0;
volatile uint32_t tempLeftNotchCount = 0;
volatile double leftTotalDistance = 0.0;
volatile double tempLeftTotalDistance = 0.0;
volatile uint64_t leftLastNotchTime = 0;
volatile double leftEncoderSpeed = 0.0;

// Global variables to store measurement data for the right wheel
volatile uint32_t rightNotchCount = 0;
volatile uint32_t tempRightNotchCount = 0;
volatile double rightTotalDistance = 0.0;
volatile double tempRightTotalDistance = 0.0;
volatile uint64_t rightLastNotchTime = 0;
volatile double rightEncoderSpeed = 0.0;

/*!
 * @brief Retrieves the current speed of the left wheel.
 *
 * @param[in] params Optional parameters (unused in this function).
 * @return The speed of the left wheel.
 */
double getLeftSpeed(void *params) {
    return leftEncoderSpeed;
}

/*!
 * @brief Retrieves the current speed of the right wheel.
 *
 * @param[in] params Optional parameters (unused in this function).
 * @return The speed of the right wheel.
 */
double getRightSpeed(void *params) {
    return rightEncoderSpeed;
}

/*!
 * @brief Retrieves the total count of notches detected by the left wheel encoder.
 *
 * @param[in] params Optional parameters (unused in this function).
 * @return The total notch count for the left wheel.
 */
uint32_t getLeftNotchCount(void *params) {
    return leftNotchCount;
}

/*!
 * @brief Retrieves the total count of notches detected by the right wheel encoder.
 *
 * @param[in] params Optional parameters (unused in this function).
 * @return The total notch count for the right wheel.
 */
uint32_t getRightNotchCount(void *params) {
    return rightNotchCount;
}

/*!
 * @brief Interrupt service routine for the left wheel encoder.
 *        Increments the notch count and calculates the speed of the left wheel.
 *
 * @param[in] params Optional parameters (unused in this function).
 */
void leftEncoder(void *params) {
    // Increment the count of notches detected for the left wheel
    leftNotchCount++;
    tempLeftNotchCount++;

    // Calculate the total distance traveled by the left wheel in centimeters
    leftTotalDistance = (double)leftNotchCount * CM_PER_NOTCH;
    tempLeftTotalDistance = (double)tempLeftNotchCount * CM_PER_NOTCH;

    // Calculate speed traveled after every cycle
    if (tempLeftNotchCount % NOTCHES_PER_CYCLE == 0) {
        // Get the current time in microseconds
        uint64_t currentTime = time_us_64();

        // Calculate the time difference between the current and previous notch
        uint64_t timeDiff = currentTime - leftLastNotchTime;

        if (timeDiff > 0) {
            // Calculate speed (distance traveled in 1 second) for the left wheel
            leftEncoderSpeed = tempLeftTotalDistance * 1e6 / timeDiff;
        }

        // Update the last notch time for the left wheel
        leftLastNotchTime = currentTime;

        tempLeftNotchCount = 0;
    }
}

/*!
 * @brief Interrupt service routine for the right wheel encoder.
 *        Increments the notch count and calculates the speed of the right wheel.
 *
 * @param[in] params Optional parameters (unused in this function).
 */
void rightEncoder(void *params) {
    // Increment the count of notches detected for the right wheel
    rightNotchCount++;
    tempRightNotchCount++;

    // Calculate the total distance traveled by the right wheel in centimeters
    rightTotalDistance = (double)rightNotchCount * CM_PER_NOTCH;
    tempRightTotalDistance = (double)tempRightNotchCount * CM_PER_NOTCH;

    // Calculate speed traveled after every cycle for the right wheel
    if (tempRightNotchCount % NOTCHES_PER_CYCLE == 0) {

        // Get the current time in microseconds
        uint64_t currentTime = time_us_64();

        // Calculate the time difference between the current and previous notch
        uint64_t timeDiff = currentTime - rightLastNotchTime;

        if (timeDiff > 0) {
            // Calculate speed (distance traveled in 1 second) for the right wheel
            rightEncoderSpeed = tempRightTotalDistance * 1e6 / timeDiff;
        }

        // Update the last notch time for the right wheel
        rightLastNotchTime = currentTime;

        tempRightNotchCount = 0;
    }
}

/*** End of file ***/
