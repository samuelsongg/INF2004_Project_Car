#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/irline.h"
#include "pico/time.h"
#include "string.h"

// Global variables for left and right IR sensor results and pulse width
volatile uint32_t l_ir_result;
volatile uint32_t r_ir_result;
volatile uint64_t ir_pulse_width;
volatile absolute_time_t start_time_ir;
volatile absolute_time_t end_time_ir;

volatile bool white_surface_detected = false;   //Idk if needed.
absolute_time_t white_surface_start_time;       // Needed.
absolute_time_t black_surface_start_time;       // Needed.
uint32_t elapsed_seconds = 0;
uint32_t last_elapsed_seconds = 0;

// Creating a Array Pointer to Store a Single Character.
static char *charArr[CHAR_ARR_SIZE];

// Creating a Variable to Store Character Array.
static char charCode[CONCATENATED];

// Creating a static Position counter.
static int currentPosition = 0;

// Creating a Variable to Store the Character String.
static char result;

// Creating a Variable to Store the Last Interrupt Event.
typedef enum {
    NONE,
    EDGE_RISE,
    EDGE_FALL
} LastInterruptEvent;

static LastInterruptEvent last_event = NONE;

// Defining a Struct for decrpyting Barcode39 Characters.
typedef struct {
    char character;
    char pattern[CONCATENATED];
} Barcode39Chars;

// Defining the Barcode39 Characters and their individual Pattern.
static const Barcode39Chars Barcode39Table[] = {
    {'*' , "100010111011101"},
    {'A' , "111010100010111"},
    {'B' , "1011101000101110"},
    {'C' , "1110111010001010"},
    {'D' , "1010111000101110"},
    {'E' , "1110101110001010"},
    {'F' , "1011101110001010"},
    {'G' , "1010100011101110"},
    {'H' , "1110101000111010"},
    {'I' , "1011101000111010"},
    {'J' , "1010111000111010"},
    {'K' , "1110101010001110"},
    {'L' , "1011101010001110"},
    {'M' , "1110111010100010"},
    {'N' , "1010111010001110"},
    {'O' , "1110101110100010"},
    {'P' , "1011101110100010"},
    {'Q' , "1010101110001110"},
    {'R' , "1110101011100010"},
    {'S' , "1011101011100010"},
    {'T' , "1010111011100010"},
    {'U' , "1110001010101110"},
    {'V' , "1000111010101110"},
    {'W' , "1110001110101010"},
    {'X' , "1000101110101110"},
    {'Y' , "1110001011101010"},
    {'Z' , "1000111011101010"},
    {'-' , "1000101011101110"},
    {'.' , "1110001010111010"},
    {'$' , "1000100010001010"},
    {'/' , "1000100010100010"},
    {'+' , "1000101000100010"},
    {'%' , "1010001000100010"},
    {' ' , "1000111010111010"},
}; 

// Defining the TABLE_SIZE of the Barcode39Table.
#define TABLE_SIZE  (sizeof(Barcode39Table) / sizeof(Barcode39Table[0]))

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
    
    // gpio_set_dir(RIGHT_IR_SENSOR_A0, GPIO_IN); // IR Sensor
    // gpio_set_function(RIGHT_IR_SENSOR_A0, GPIO_FUNC_SIO);
    // gpio_set_pulls(RIGHT_IR_SENSOR_A0, false, true); // pull down high state when on black

    gpio_init(RIGHT_IR_SENSOR_A0);
    gpio_set_dir(RIGHT_IR_SENSOR_A0, GPIO_IN);

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
}

void read_pulse_width(void *params) {

    // Measure time to detect IR signals exceeding the color cutoff value
    start_time_ir = get_absolute_time();
    // adc_select_input(0);
    // l_ir_result = adc_read();
    adc_select_input(1);
    r_ir_result = adc_read();
    
    // Measure pulse width when IR signal ends
    end_time_ir = get_absolute_time();
    ir_pulse_width = absolute_time_diff_us(start_time_ir, end_time_ir); // Todo: convert to cm.
}

// Test Function to Print Array.
static void printArray(char **arr) 
{
    printf("\nArray Contents: ");
    for (int i = 0; i < CHAR_ARR_SIZE; i++) {
        if (arr[i]) {
            printf("%s ", arr[i]);
        } else {
            printf("(null) ");  // for null pointers
        }
    }
    printf("\n");
}

// Function to Append Values to a Pointer Array.
void appendArrayValue(char **arr, const char *value) 
{
    // Shift all elements down by one
    for (int i = 0; i < CHAR_ARR_SIZE - 1; i++) {
        arr[i] = arr[i + 1];
    }

    // Insert new value at the latest index
    arr[CHAR_ARR_SIZE - 1] = (char *)value;

    // printf("Inserted.\n");
    printArray(arr);
}

// Function to Concatenate the Array Values to a single String.
char* concatenateArrayValue(char **arr)
{
    // Initialize the string as empty.
    charCode[0] = '\0';

    for (int i = 0; i < CHAR_ARR_SIZE; i++)
    {
        if (arr[i])
        {
            strcat(charCode, arr[i]);
        }
    }
}

// Function to check Character String with the Barcode List.
char checkPattern(const char *pattern)
{
    for (size_t i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(Barcode39Table[i].pattern, pattern) == 0) {
            return Barcode39Table[i].character;
        }
    }
    return '\0';  // Return null character if not found
}

// Interrupt! Edge Rise / Edge Fall.
void white_surface_detected_handler(uint gpio, uint32_t events)
{
    // Check for consecutive Edge Falls
    if ((events & GPIO_IRQ_EDGE_FALL) && last_event == EDGE_FALL) {
        return; // Ignore this interrupt
    }

    // Check for consecutive Edge Rises
    if ((events & GPIO_IRQ_EDGE_RISE) && last_event == EDGE_RISE) {
        return; // Ignore this interrupt
    }

    // This Event detects a White Bar.
    if (events & GPIO_IRQ_EDGE_FALL)
    {
        last_event = EDGE_FALL;
        /*  If it detects White as the 1st Position of the new Array, 
            It should be ignored as the Barcode39 starts with a Black at All Times. */
        if(currentPosition == 0)
        {
            // Do nothing.
        }
        else
        {
            // Check if the Value is a Wide or Thin Bar, based on the running average and thresholds.
            if ( (elapsed_seconds > THIN_THRESHOLD) && (elapsed_seconds < WIDE_THRESHOLD) )
            {
                // Append the Wide Value into the charArr.
                appendArrayValue(charArr, WIDE_WHITE_BAR);
            }
            else if (elapsed_seconds < THIN_THRESHOLD)
            {
                // Append the Thin Value into the charArr.
                appendArrayValue(charArr, THIN_WHITE_BAR);
            }
            else
            {
                // Append a ? Value into the charArr to show running average is not accurate.
                appendArrayValue(charArr, "?");
            }

            white_surface_detected = true;
            white_surface_start_time = get_absolute_time();

            // Increment the current position.
            currentPosition++;

            concatenateArrayValue(charArr);
            // printf("\nCharacter Code is: %s\n\n", charCode);
            result = checkPattern(charCode);
            if (result != '\0')
            {
                printf("Pattern Detected %s! The Character is %c\n", charCode, result);
                currentPosition = 0;
            }

            // Check if we've reached the end of the array
            if (currentPosition >= CHAR_ARR_SIZE)
            {
                currentPosition = 0;  // Wrap around to the start
            }

        }
    }
    // This Event detects a Black Bar.
    else if (events & GPIO_IRQ_EDGE_RISE)
    {
        last_event = EDGE_RISE;
        /*  If it detects Black as the last Position of the Array, 
            It should be ignored as the Barcode39 ends with a White at All Times. */
        if (currentPosition == 10)
        {
            // Do nothing.
        }
        else
        {
            // Check if the Value is a Wide or Thin Bar, based on the running average and thresholds.
            if ( (elapsed_seconds > THIN_THRESHOLD) && (elapsed_seconds < WIDE_THRESHOLD) )
            {
                // Append the Wide Value into the charArr.
                appendArrayValue(charArr, WIDE_BLACK_BAR);
            }
            else if (elapsed_seconds < THIN_THRESHOLD)
            {
                // Append the Thin Value into the charArr.
                appendArrayValue(charArr, THIN_BLACK_BAR);
            }
            else
            {
                // Append a ? Value into the charArr to show running average is not accurate.
                appendArrayValue(charArr, "?");
            }

            white_surface_detected = false;
            black_surface_start_time = get_absolute_time();

            // Increment the current position.
            currentPosition++;

            concatenateArrayValue(charArr);
            // printf("\nCharacter Code is: %s\n\n", charCode);
            result = checkPattern(charCode);
            if (result != '\0')
            {
                printf("Pattern Detected %s! The Character is %c\n", charCode, result);
                currentPosition = 0;
            }

            // Check if we've reached the end of the array
            if (currentPosition >= CHAR_ARR_SIZE)
            {
                currentPosition = 0;  // Wrap around to the start
                // printf("\nImpossible!!!\tERROR!!!\tERROR!!!\tERROR!!!\tERROR!!!\n\n");
            }
        }
    }
}

void ir_main_loop(void *params)
{
    if (white_surface_detected)
    {
        absolute_time_t current_time = get_absolute_time();
        elapsed_seconds = absolute_time_diff_us(white_surface_start_time, current_time) / 1000;

        // printf("WHITE -- Time elapsed: %d ms\n", elapsed_seconds);
        // printf("Current Counter: %d\n", currentPosition);
    }
    else
    {
        absolute_time_t current_time = get_absolute_time();
        elapsed_seconds = absolute_time_diff_us(black_surface_start_time, current_time) / 1000;

        // printf("BLACK -- Time elapsed: %d ms\n", elapsed_seconds);
        // printf("Current Counter: %d\n", currentPosition);
    }
}

/* End of file */
