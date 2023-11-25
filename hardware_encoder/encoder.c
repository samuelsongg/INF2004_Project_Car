#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/encoder.h"

volatile uint32_t leftNotchCount = 0;
volatile uint32_t tempLeftNotchCount = 0;
volatile double leftTotalDistance = 0.0;
volatile double tempLeftTotalDistance = 0.0;
volatile uint64_t leftLastNotchTime = 0;
volatile double leftEncoderSpeed;

volatile uint32_t rightNotchCount = 0;
volatile uint32_t tempRightNotchCount = 0;
volatile double rightTotalDistance = 0.0;
volatile double tempRightTotalDistance = 0.0;
volatile uint64_t rightLastNotchTime = 0;
volatile double rightEncoderSpeed;

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