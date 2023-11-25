#include <stdio.h>
#include "hardware/motor.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

uint slice_num_left;
uint slice_num_right;
volatile int left_level = 0;
volatile int right_level = 0;

// Function to initialize the motor
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

// Function to set speed for the left motor
void setLeftSpeed(float speed_multiplier) {
    pwm_set_clkdiv(slice_num_left, CLK_DIV);
    pwm_set_wrap(slice_num_left, PWM_WRAP);
    pwm_set_chan_level(slice_num_left, PWM_CHAN_A, PWM_WRAP * speed_multiplier);
    left_level = PWM_WRAP * speed_multiplier;
    pwm_set_enabled(slice_num_left, true);
}

void increaseLeftSpeed(void *params) {
    left_level += 7;
    pwm_set_chan_level(slice_num_left, PWM_CHAN_A, left_level);
}

void decreaseLeftSpeed(void *params) {
    left_level -= 7;
    pwm_set_chan_level(slice_num_left, PWM_CHAN_A, left_level);
}

// Function to set speed for the right motor
void setRightSpeed(float speed_multiplier) {
    pwm_set_clkdiv(slice_num_right, CLK_DIV);
    pwm_set_wrap(slice_num_right, 10000);
    pwm_set_chan_level(slice_num_right, PWM_CHAN_B, PWM_WRAP * speed_multiplier);
    right_level = PWM_WRAP * speed_multiplier;
    pwm_set_enabled(slice_num_right, true);
}

void increaseRightSpeed(void *params) {
    right_level++;
    pwm_set_chan_level(slice_num_right, PWM_CHAN_B, right_level);
}

void decreaseRightSpeed(void *params) {
    right_level--;
    pwm_set_chan_level(slice_num_right, PWM_CHAN_B, right_level);
}

// Function to stop the motors
void stop(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 0);
    gpio_put(RIGHT_WHEEL_BACKWARD, 0);
    gpio_put(LEFT_WHEEL_FORWARD, 0);
    gpio_put(LEFT_WHEEL_BACKWARD, 0);
}

// Functions for directional movement of the robot
void moveForward(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 1);
    gpio_put(RIGHT_WHEEL_BACKWARD, 0);
    gpio_put(LEFT_WHEEL_FORWARD, 1);
    gpio_put(LEFT_WHEEL_BACKWARD, 0);
}

void moveBackward(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 0);
    gpio_put(RIGHT_WHEEL_BACKWARD, 1);
    gpio_put(LEFT_WHEEL_FORWARD, 0);
    gpio_put(LEFT_WHEEL_BACKWARD, 1);
}

void turnHardLeft(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 1);
    gpio_put(RIGHT_WHEEL_BACKWARD, 0);
    gpio_put(LEFT_WHEEL_FORWARD, 0);
    gpio_put(LEFT_WHEEL_BACKWARD, 1);
}

void turnHardRight(void *params) {
    gpio_put(RIGHT_WHEEL_FORWARD, 0);
    gpio_put(RIGHT_WHEEL_BACKWARD, 1);
    gpio_put(LEFT_WHEEL_FORWARD, 1);
    gpio_put(LEFT_WHEEL_BACKWARD, 0);
}

/*** End of file ***/