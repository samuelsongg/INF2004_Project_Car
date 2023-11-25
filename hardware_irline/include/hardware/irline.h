#ifndef _IRLINE_H
#define _IRLINE_H

// Define GPIO pins for IR sensors and their respective ground pins
#define LEFT_IR_SENSOR_A0 26
#define LEFT_IR_SENSOR_GND 3
#define RIGHT_IR_SENSOR_A0 27
#define RIGHT_IR_SENSOR_GND 5

// Constants for color differentiation and pulse width timeout
#define COLOUR_CUTOFF_VALUE 1000 // Color difference between black and white. Use IR sensor to measure.
#define PULSE_WIDTH_TIMEOUT 1000

// Declare global variables for IR sensor results and pulse width
extern volatile uint32_t l_ir_result;
extern volatile uint32_t r_ir_result;
extern volatile uint64_t ir_pulse_width;

// Function prototypes for IR sensor setup and reading functions
void ir_setup(void *params);
void read_ir(void *params);

#endif

/*** End of file ***/
