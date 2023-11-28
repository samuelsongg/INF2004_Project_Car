/** @file motor.h
 *
 * @brief This header file declares functions and macros for controlling 
 * the motors of the robotic car, including initialization, speed control, 
 * and directional movement.
 */

#ifndef _MOTOR_H
#define _MOTOR_H

// Definitions of GPIO pins for motor control
#define LEFT_WHEEL 14
#define RIGHT_WHEEL 15
#define RIGHT_WHEEL_FORWARD 18
#define RIGHT_WHEEL_BACKWARD 19
#define LEFT_WHEEL_FORWARD 20
#define LEFT_WHEEL_BACKWARD 21

// Constants for PWM settings
#define CLK_DIV 100
#define PWM_WRAP 12500

// Function declarations for motor control
void initMotor(void *params);
void setLeftSpeed(float speed_multiplier);
void increaseLeftSpeed(void *params);
void decreaseLeftSpeed(void *params);
void setRightSpeed(float speed_multiplier);
void increaseRightSpeed(void *params);
void decreaseRightSpeed(void *params);
void stop(void *params);
void moveForward(void *params);
void moveBackward(void *params);
void turnHardLeft(void *params);
void turnHardRight(void *params);

#endif /* _MOTOR_H */

/*** End of file ***/
