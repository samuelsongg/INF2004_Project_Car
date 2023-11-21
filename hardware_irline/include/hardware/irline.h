#ifndef _IRLINE_H
#define _IRLINE_H

// Define GPIO pins for IR sensors and their respective ground pins
#define LEFT_IR_SENSOR_A0 26
#define LEFT_IR_SENSOR_GND 3
#define RIGHT_IR_SENSOR_A0 27
#define RIGHT_IR_SENSOR_GND 5

// Constants for color differentiation and pulse width timeout
#define COLOUR_CUTOFF_VALUE 650 // Color difference between black and white. Use IR sensor to measure.
#define PULSE_WIDTH_TIMEOUT 1000

#define CHAR_ARR_SIZE       10
#define CONCATENATED        31
#define THIN_THRESHOLD      500                // Adjustable in ms. ADJUST THIS ALAN CB DOG
#define WIDE_THRESHOLD      1000               // Adjustable in ms. ADJUST THIS ALAN CB DOG 
#define THIN_WHITE_BAR      "1"
#define WIDE_WHITE_BAR      "111"
#define THIN_BLACK_BAR      "0"
#define WIDE_BLACK_BAR      "000"

// Declare global variables for IR sensor results and pulse width
extern volatile uint32_t l_ir_result;
extern volatile uint32_t r_ir_result;
extern volatile uint64_t ir_pulse_width;
extern volatile char *thickness;

// Function prototypes for IR sensor setup and reading functions
void ir_setup(void *params);
void read_ir(void *params);
void read_pulse_width(void *params);
void ir_main_loop(void *params);
void white_surface_detected_handler(uint gpio, uint32_t events);
char checkPattern(const char *pattern);
char* concatenateArrayValue(char **arr);
void appendArrayValue(char **arr, const char *value);
static void printArray(char **arr);


#endif

/*** End of file ***/
