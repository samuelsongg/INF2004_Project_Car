/**
 * @file barcode.h
 *
 * @brief Provides definitions and declarations for barcode processing.
 *
 * This header file defines constants and declares functions for barcode processing.
 * It includes the setup of ADC and GPIO for barcode detection, functions for classifying
 * the barcode signals, and utility functions for barcode data handling. The file supports
 * barcode processing and interpretation based on the Code 39 barcode standard.
 *
 */

#ifndef _BARCODE_H
#define _BARCODE_H

#include <stddef.h>

// Constants for barcode processing
#define TABLE_SIZE 100
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define ADC_PIN 26
#define DIGITAL_PIN 22
#define BLACK_THRESHOLD 1000
#define WHITE_THRESHOLD 400
#define BARCODE_BUF_SIZE 10
#define BARCODE_ARR_SIZE 9
#define ADC_DIFFERENCE_THRESHHOLD 50
#define SAMPLE_SIZE 10000

struct voltageClassification;

void barcode_setup();
void barcode_main_loop();
static int* thickThinClassification();
static int isVoltageClassificationFull();
static void flushVoltageClassification();
static char compareTwoArray ();
static void appendVoltageClassification(struct voltageClassification voltageClassification);
static void ADC_IRQ_FIFO_HANDLER();
char getBarcodeChar();

#endif

/*** End of file ***/