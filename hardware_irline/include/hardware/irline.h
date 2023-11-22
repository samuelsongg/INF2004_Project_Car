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

#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define ADC_PIN 26
#define DIGITAL_PIN 22
#define BLACK_THRESHOLD 3000
#define WHITE_THRESHOLD 400
#define BARCODE_BUF_SIZE 10
#define BARCODE_ARR_SIZE 9
#define ADC_DIFFERENCE_THRESHHOLD 50
#define SAMPLE_SIZE 10000

// Declare global variables for IR sensor results and pulse width
extern volatile uint32_t l_ir_result;
extern volatile uint32_t r_ir_result;
extern volatile uint64_t ir_pulse_width;
extern volatile char *thickness;

extern struct voltageClassification voltageClassification;
extern uint8_t barcodeSecondChar;

// static struct voltageClassification voltageClassifications[BARCODE_BUF_SIZE];

// Function prototypes for IR sensor setup and reading functions
void ir_setup(void *params);
void read_ir(void *params);
void ir_main_loop();
static int* thickThinClassification();
static int isVoltageClassificationFull();
static void flushVoltageClassification();
static char compareTwoArray ();
static void appendVoltageClassification(struct voltageClassification voltageClassification);
static void ADC_IRQ_FIFO_HANDLER();


#endif

/*** End of file ***/
