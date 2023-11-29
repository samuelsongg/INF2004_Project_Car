#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/irline.h"

// Global variables for left and right IR sensor results and pulse width
volatile uint32_t l_ir_result;
volatile uint32_t r_ir_result;
volatile uint64_t ir_pulse_width;

// Setup function for IR sensors
void ir_setup(void *params) {
    // Initialize ADC and enable the temperature sensor
    adc_init();
    adc_set_temp_sensor_enabled(true);

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
    gpio_disable_pulls(LEFT_IR_SENSOR_A0);
    gpio_set_input_enabled(LEFT_IR_SENSOR_A0, false);
    
    gpio_set_dir(RIGHT_IR_SENSOR_A0, GPIO_IN); // IR Sensor
    gpio_set_function(RIGHT_IR_SENSOR_A0, GPIO_FUNC_SIO);
    gpio_disable_pulls(RIGHT_IR_SENSOR_A0);
    gpio_set_input_enabled(RIGHT_IR_SENSOR_A0, false);

    // Initialize hash map (If applicable)
    // initHashMap(NULL); // Insert key-value pairs for barcode scanner.
}

// Function to read IR sensor values and calculate pulse width
void read_ir(void *params) {
    // Read ADC values from left and right IR sensors
    adc_select_input(0);
    l_ir_result = adc_read();
    adc_select_input(1);
    r_ir_result = adc_read();

    // bool start = false;

    // // Measure time to detect IR signals exceeding the color cutoff value
    
    // if (l_ir_result > COLOUR_CUTOFF_VALUE && r_ir_result > COLOUR_CUTOFF_VALUE) {
    //     absolute_time_t start_time = get_absolute_time();
    //     adc_select_input(0);
    //     l_ir_result = adc_read();
    //     adc_select_input(1);
    //     r_ir_result = adc_read();
    // }
    
    // // Measure pulse width when IR signal ends
    // absolute_time_t end_time = get_absolute_time();
    // ir_pulse_width = absolute_time_diff_us(start_time, end_time); // Todo: convert to cm.
}

/* End of file */
