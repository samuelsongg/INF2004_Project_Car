#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LEFT_ENCODER_PIN 16
#define RIGHT_ENCODER_PIN 17
#define CM_PER_NOTCH 1.05

volatile uint32_t leftNotchCount = 0;
volatile double leftTotalDistance = 0.0;
volatile uint64_t leftLastNotchTime = 0;

volatile uint32_t rightNotchCount = 0;
volatile double rightTotalDistance = 0.0;
volatile uint64_t rightLastNotchTime = 0;

// Function to calculate the distance traveled for the left wheel
void calculateLeftDistance()
{
    // Increment the count of notches detected for the left wheel
    leftNotchCount++;

    // Calculate the total distance traveled by the left wheel in centimeters
    leftTotalDistance = (double)leftNotchCount * CM_PER_NOTCH;
    printf("LEFT Total Distance: %.2f cm\n", leftTotalDistance);
}

// Function to calculate the speed for the left wheel
void calculateLeftSpeed(uint64_t timeDiff)
{
    if (timeDiff > 0)
    {
        // Calculate and print speed (distance traveled in 1 second) for the left wheel
        double speed_cm_per_s = CM_PER_NOTCH * 1e6 / timeDiff;
        printf("LEFT Current Speed: %.2f cm/s\n", speed_cm_per_s);
    }
}

// Function to calculate the distance traveled for the right wheel
void calculateRightDistance()
{
    // Increment the count of notches detected for the right wheel
    rightNotchCount++;

    // Calculate the total distance traveled by the right wheel in centimeters
    rightTotalDistance = (double)rightNotchCount * CM_PER_NOTCH;
    printf("RIGHT Total Distance: %.2f cm\n", rightTotalDistance);
}

// Function to calculate the speed for the right wheel
void calculateRightSpeed(uint64_t timeDiff)
{
    if (timeDiff > 0)
    {
        // Calculate and print speed (distance traveled in 1 second) for the right wheel
        double speed_cm_per_s = CM_PER_NOTCH * 1e6 / timeDiff;
        printf("RIGHT Current Speed: %.2f cm/s\n", speed_cm_per_s);
    }
}

// Callback function to handle GPIO events (falling edge to next falling edge) for wheel encoder
void gpio_callback(uint gpio, uint32_t events)
{
    static uint64_t fallingEdgeTimeLeft = 0;
    static uint64_t fallingEdgeTimeRight = 0;

    if (gpio == LEFT_ENCODER_PIN)
    {
        if (events & GPIO_IRQ_EDGE_FALL)
        {
            if (fallingEdgeTimeLeft == 0)
            {
                fallingEdgeTimeLeft = time_us_64();
            }
            else
            {
                uint64_t currentTime = time_us_64();

                // Calculate the time difference between the current falling edge and the previous falling edge for the left wheel
                uint64_t timeDiff = currentTime - fallingEdgeTimeLeft;

                calculateLeftDistance();

                calculateLeftSpeed(timeDiff);

                fallingEdgeTimeLeft = currentTime; // Update falling edge time for the next measurement
            }
        }
    }
    else if (gpio == RIGHT_ENCODER_PIN)
    {
        if (events & GPIO_IRQ_EDGE_FALL)
        {
            if (fallingEdgeTimeRight == 0)
            {
                fallingEdgeTimeRight = time_us_64();
            }
            else
            {
                uint64_t currentTime = time_us_64();

                // Calculate the time difference between the current falling edge and the previous falling edge for the right wheel
                uint64_t timeDiff = currentTime - fallingEdgeTimeRight;

                calculateRightDistance();

                calculateRightSpeed(timeDiff);
                
                fallingEdgeTimeRight = currentTime; // Update falling edge time for the next measurement
            }
        }
    }
}

// Function to set up wheel encoders for both left and right wheels
void setUpWheelEncoders()
{
    // Set up the GPIO pins to trigger the gpio_callback function to calculate distance and speed for both wheels on both rising and falling edges
    gpio_set_irq_enabled_with_callback(LEFT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(RIGHT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

int main()
{
    stdio_init_all();

    printf("Wheel Encoder Speed and Distance\n");

    // Set up the encoder GPIO pins
    setUpWheelEncoders();

    // Enter an infinite loop to keep the program running
    while (1)
        ;
}