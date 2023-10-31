#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LEFT_ENCODER_PIN 16
#define RIGHT_ENCODER_PIN 17

#define NOTCHES_PER_CYCLE 20
#define CM_PER_NOTCH 1.0

volatile uint32_t leftNotchCount = 0;
volatile double leftTotalDistance = 0.0;
volatile uint64_t leftLastNotchTime = 0;

volatile uint32_t rightNotchCount = 0;
volatile double rightTotalDistance = 0.0;
volatile uint64_t rightLastNotchTime = 0;


void calculateLeftDistance()
{

    // Increment the count of notches detected for the left wheel
    leftNotchCount++;

    // Calculate the total distance traveled by the left wheel in centimeters
    leftTotalDistance = (double)leftNotchCount * CM_PER_NOTCH;
    printf("LEFT Total Distance: %.2f cm\n", leftTotalDistance);

}

void calculateLeftSpeed()
{
    // Calculate speed traveled after every cycle
    if (leftNotchCount % NOTCHES_PER_CYCLE == 0)
    {
        // Get the current time in microseconds
        uint64_t currentTime = time_us_64();

        // Calculate the time difference between the current and previous notch
        uint64_t timeDiff = currentTime - leftLastNotchTime;

        if (timeDiff > 0)
        {
            // Calculate and print speed (distance traveled in 1 second) for the left wheel
            double speed_cm_per_s = leftTotalDistance * 1e6 / timeDiff;
            printf("LEFT Current Speed: %.2f cm/s\n", speed_cm_per_s);
        }

        // Update the last notch time for the left wheel
        leftLastNotchTime = currentTime;
    }
}

void calculateRightDistance()
{
    
    // Increment the count of notches detected for the right wheel
    rightNotchCount++;

    // Calculate the total distance traveled by the right wheel in centimeters
    rightTotalDistance = (double)rightNotchCount * CM_PER_NOTCH;
    printf("RIGHT Total Distance: %.2f cm\n", rightTotalDistance);

}

void calculateRightSpeed()
{
    // Calculate speed traveled after every cycle for the right wheel
    if (rightNotchCount % NOTCHES_PER_CYCLE == 0)
    {
        // Get the current time in microseconds
        uint64_t currentTime = time_us_64();

        // Calculate the time difference between the current and previous notch
        uint64_t timeDiff = currentTime - rightLastNotchTime;

        if (timeDiff > 0)
        {
            // Calculate and print speed (distance traveled in 1 second) for the right wheel
            double speed_cm_per_s = rightTotalDistance * 1e6 / timeDiff;
            printf("RIGHT Current Speed: %.2f cm/s\n", speed_cm_per_s);
        }

        // Update the last notch time for the right wheel
        rightLastNotchTime = currentTime;
    }
    
}

// Callback function to handle GPIO events (rising and falling edges) for wheel encoder 
void gpio_callback(uint gpio, uint32_t events)
{
    // Check if a rising or falling edge event occurred
    if (events & (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL))
    {
        // If left wheel is moving
        if (gpio == LEFT_ENCODER_PIN)
        {
            calculateLeftDistance();
            calculateLeftSpeed();
            
        }

        // If the right wheel is moving
        else if (gpio == RIGHT_ENCODER_PIN)
        {
            calculateRightDistance();
            calculateRightSpeed();
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
