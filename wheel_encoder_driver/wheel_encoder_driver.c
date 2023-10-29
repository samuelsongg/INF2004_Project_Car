#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define ENCODER_PIN 2

#define NOTCHES_PER_CYCLE 20
#define CM_PER_NOTCH 1.0

volatile uint32_t notchCount = 0;
volatile double totalDistance = 0.0;
volatile uint64_t lastNotchTime = 0;

void updateDistance()
{
    // Calculate total distance based on the number of notches
    totalDistance = (double)notchCount * CM_PER_NOTCH;
    printf("Total Distance: %.2f cm\n", totalDistance);
}

void updateSpeed()
{
    uint64_t currentTime = time_us_64();

    /* Calculate the time difference between the current time (the time when the latest notch was detected) 
    & the time when the previous notch was detected */ 
    
    uint64_t timeDiff = currentTime - lastNotchTime;


    if (timeDiff > 0)
    {
        // Calculate and print speed (distance traveled in 1 second)
        double speed_cm_per_s = totalDistance * 1e6 / timeDiff;
        printf("Current Speed: %.2f cm/s\n", speed_cm_per_s);
    }

    lastNotchTime = currentTime;
}

void gpio_callback(uint gpio, uint32_t events)
{
    if (events & (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL))
    {
        notchCount++;
        updateDistance();

        // Calculate speed travelled after every cycle
        
        if (notchCount % 20 == 0){
            
            updateSpeed();
        }
        
    }
}

void setupEncoder()
{
    // Set up the GPIO pin (ENCODER_PIN) to trigger the gpio_callback function on both rising and falling edges
    gpio_set_irq_enabled_with_callback(ENCODER_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

int main()
{
    // Initialize standard I/O for printing
    stdio_init_all();

    // Print a message indicating the program's purpose
    printf("Wheel Encoder Speed and Distance\n");

    // Set up the encoder GPIO pin
    setupEncoder();

    // Enter an infinite loop to keep the program running
    while (1)
        ;
}
