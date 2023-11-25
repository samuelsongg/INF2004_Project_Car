#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/irline.h"

volatile uint32_t l_ir_result;
volatile uint32_t r_ir_result;
volatile uint64_t ir_pulse_width;
volatile absolute_time_t start_time_ir;
volatile absolute_time_t end_time_ir;

// Function to read IR sensor values and calculate pulse width
void read_ir(void *params) {
    // Read ADC values from left and right IR sensors
    adc_select_input(0);
    l_ir_result = adc_read();
    adc_select_input(1);
    r_ir_result = adc_read();
}

// Setup function for IR sensors
void ir_setup(void *params) {
    // Initialize GPIO pins for IR sensors' GND
    gpio_init(LEFT_IR_SENSOR_GND);
    gpio_init(RIGHT_IR_SENSOR_GND);

    // Set GPIO directions for IR sensors' GND pins
    gpio_set_dir(LEFT_IR_SENSOR_GND, GPIO_OUT);
    gpio_set_dir(RIGHT_IR_SENSOR_GND, GPIO_OUT);

    // Set LEFT and RIGHT IR sensor GND pins to low
    gpio_put(LEFT_IR_SENSOR_GND, 0); // Set to low for LEFT_IR_Sensor's GND
    gpio_put(RIGHT_IR_SENSOR_GND, 0); // Set to low for RIGHT_IR_Sensor's GND

    // Initialize GPIO pins for IR sensors
    gpio_set_dir(LEFT_IR_SENSOR_A0, GPIO_IN); // IR Sensor
    gpio_set_function(LEFT_IR_SENSOR_A0, GPIO_FUNC_SIO);

    gpio_init(RIGHT_IR_SENSOR_A0);
    gpio_set_dir(RIGHT_IR_SENSOR_A0, GPIO_IN);
}

/*** End of file ***/