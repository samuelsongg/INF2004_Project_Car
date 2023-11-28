/** @file motor.c
 *
 * @brief This module handles motor control functions including initialization,
 * speed setting, and directional movement for the robotic car.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2023 Team 61. All rights reserved.
 */

#include <stdio.h>
#include "hardware/motor.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// Global variables to store measurement data
uint slice_num_left;
uint slice_num_right;
volatile int left_level = 0;
volatile int right_level = 0;

/**
 * Initializes the motor control system by configuring GPIO pins and PWM slices.
 *
 * @param params Optional parameters (unused in this function).
 */
void initMotor(void *params) {
    // Setting up GPIO functions
    gpio_set_function(LEFT_WHEEL, GPIO_FUNC_PWM);
    gpio_set_function(RIGHT_WHEEL, GPIO_FUNC_PWM);

    // Assigning PWM slice numbers
    slice_num_left = pwm_gpio_to_slice_num(LEFT_WHEEL);
    slice_num_right = pwm_gpio_to_slice_num(RIGHT_WHEEL);

    // Initializing motor control pins
    gpio_init(RIGHT_WHEEL_FORWARD);
    gpio_init(RIGHT_WHEEL_BACKWARD);
    gpio_init(LEFT_WHEEL_FORWARD);
    gpio_init(LEFT_WHEEL_BACKWARD);

    // Setting motor control pins direction
    gpio_set_dir(RIGHT_WHEEL_FORWARD, GPIO_OUT);
    gpio_set_dir(RIGHT_WHEEL_BACKWARD, GPIO_OUT);
    gpio_set_dir(LEFT_WHEEL_FORWARD, GPIO_OUT);
    gpio_set_dir(LEFT_WHEEL_BACKWARD, GPIO_OUT);
}

/**
 * Sets the speed of the left motor.
 *
 * @param speed_multiplier Multiplier for PWM duty cycle.
 */
void setLeftSpeed(float speed_multiplier) {
    pwm_set_clkdiv(slice_num_left, CLK_DIV);
    pwm_set_wrap(slice_num_left, PWM_WRAP);
    pwm_set_chan_level(slice_num_left, PWM_CHAN_A, PWM_WRAP * speed_multiplier);
    left_level = PWM_WRAP * speed_multiplier;
    pwm_set_enabled(slice_num_left, true);
}

/**
 * Increases the speed of the left motor.
 *
 * @param params Optional parameters (unused in this function).
 */
void increaseLeftSpeed(void *params) {
    left_level += 7;
    pwm_set_chan_level(slice_num_left, PWM_CHAN_A, left_level);
}

/**
 * Decreases the speed of the left motor.
 *
 * @param params Optional parameters (unused in this function).
 */
void decreaseLeftSpeed(void *params) {
    left_level -= 7;
    pwm_set_chan_level(slice_num_left, PWM_CHAN_A, left_level);
}

/**
 * Sets the speed of the right motor.
 *
 * @param speed_multiplier Multiplier for PWM duty cycle.
 */
void setRightSpeed(float speed_multiplier) {
    pwm_set_clkdiv(slice_num_right, CLK_DIV);
    pwm_set_wrap(slice_num_right, 10000);
    pwm_set_chan_level(slice_num_right, PWM_CHAN_B, PWM_WRAP * speed_multiplier);
    right_level = PWM_WRAP * speed_multiplier;
    pwm_set_enabled(slice_num_right, true);
}

/**
 * Increases the speed of the right motor.
 *
 * @param params Optional parameters (unused in this function).
 */
void increaseRightSpeed(void *params) {
    right_level++;
    pwm_set_chan_level(slice_num_right, PWM_CHAN_B, right_level);
}

/**
 * Decreases the speed of the right motor.
 *
 * @param params Optional parameters (unused in this function).
 */
void decreaseRightSpeed(void *params) {
    right_level--;
    pwm_set_chan_level(slice_num_right, PWM_CHAN_B, right_level);
}

/**
 * Stops both motors.
 *
 * @param params Optional parameters (unused in this function).
 */
void stop(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 0);
    gpio_put(RIGHT_WHEEL_BACKWARD, 0);
    gpio_put(LEFT_WHEEL_FORWARD, 0);
    gpio_put(LEFT_WHEEL_BACKWARD, 0);
}

/**
 * Moves the car forward.
 *
 * @param params Optional parameters (unused in this function).
 */
void moveForward(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 1);
    gpio_put(RIGHT_WHEEL_BACKWARD, 0);
    gpio_put(LEFT_WHEEL_FORWARD, 1);
    gpio_put(LEFT_WHEEL_BACKWARD, 0);
}

/**
 * Moves the car backward.
 *
 * @param params Optional parameters (unused in this function).
 */
void moveBackward(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 0);
    gpio_put(RIGHT_WHEEL_BACKWARD, 1);
    gpio_put(LEFT_WHEEL_FORWARD, 0);
    gpio_put(LEFT_WHEEL_BACKWARD, 1);
}

/**
 * Turns the car hard left.
 *
 * @param params Optional parameters (unused in this function).
 */
void turnHardLeft(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 1);
    gpio_put(RIGHT_WHEEL_BACKWARD, 0);
    gpio_put(LEFT_WHEEL_FORWARD, 0);
    gpio_put(LEFT_WHEEL_BACKWARD, 1);
}

/**
 * Turns the car hard right.
 *
 * @param params Optional parameters (unused in this function).
 */
void turnHardRight(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 0);
    gpio_put(RIGHT_WHEEL_BACKWARD, 1);
    gpio_put(LEFT_WHEEL_FORWARD, 1);
    gpio_put(LEFT_WHEEL_BACKWARD, 0);
}

/*** End of file ***/
