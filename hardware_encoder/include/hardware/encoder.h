/** @file encoder.h
 *
 * @brief This module provides functions and definitions for working with
 * wheel encoders on the robotic car, including functions to measure
 * speed and distance traveled.
 */

#ifndef _ENCODER_H
#define _ENCODER_H

// Definitions for encoder GPIO pins and constants for calculations
#define LEFT_ENCODER_PIN 16
#define RIGHT_ENCODER_PIN 17
#define NOTCHES_PER_CYCLE 20
#define CM_PER_NOTCH 1.0

// Function declarations for encoder operations
void leftEncoder(void *params);
void rightEncoder(void *params);
double getLeftSpeed(void *params);
double getRightSpeed(void *params);
uint32_t getLeftNotchCount(void *params);
uint32_t getRightNotchCount(void *params);


#endif /* _ENCODER_H */

/*** End of file ***/
