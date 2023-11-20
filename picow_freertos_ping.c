// Standard libraries.
#include <stdio.h>
#include <stdlib.h> // for barcode codes
#include <string.h> // for barcode codes
#include <stddef.h> // for barcode codes

// Pico libraries.
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/ip4_addr.h"

// FreeRTOS libraries.
#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"
#include "message_buffer.h"

// Hardware libraries.
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"

// Sensor libraries.
#include "hardware/motor.h"
#include "hardware/ultrasonic.h"
#include "hardware/encoder.h"
#include "hardware/irline.h"

#define mbaTASK_MESSAGE_BUFFER_SIZE       ( 60 )

static MessageBufferHandle_t sendDataLeftIRSensorCMB;
static MessageBufferHandle_t sendDataRightIRSensorCMB;
static MessageBufferHandle_t sendDataUltrasonicSensorCMB;

void move_wheels(__unused void *params) {
    initMotor(NULL);

    int left_IR_data = 0;
    int right_IR_data = 0;
    int ultrasonic_data = 0;

    while (true) {
        // Insert Car Movement Algo...
        setLeftSpeed(0.5);
        setRightSpeed(0.5);

        printf("LeftIR: %d, RightIR: %d\n", left_IR_data, right_IR_data);
        printf("Ultrasonic: %d\n", ultrasonic_data);
        if (ultrasonic_data < 15) {
            stop(NULL);
        } else if (left_IR_data < COLOUR_CUTOFF_VALUE && right_IR_data < COLOUR_CUTOFF_VALUE){
            moveForward(NULL);
        } else if (left_IR_data < COLOUR_CUTOFF_VALUE && right_IR_data > COLOUR_CUTOFF_VALUE){
            turnHardLeft(NULL);
        } else if (left_IR_data > COLOUR_CUTOFF_VALUE && right_IR_data < COLOUR_CUTOFF_VALUE){
            turnHardRight(NULL);
        } else if (left_IR_data > COLOUR_CUTOFF_VALUE && right_IR_data > COLOUR_CUTOFF_VALUE){
            moveBackward(NULL);
        } else {
            stop(NULL);
        }
        

        xMessageBufferReceive(
            sendDataLeftIRSensorCMB,
            (void *) &left_IR_data,
            sizeof(left_IR_data),
            portMAX_DELAY);

        xMessageBufferReceive(
            sendDataRightIRSensorCMB,
            (void *) &right_IR_data,
            sizeof(right_IR_data),
            portMAX_DELAY);

        xMessageBufferReceive(
            sendDataUltrasonicSensorCMB,
            (void *) &ultrasonic_data,
            sizeof(ultrasonic_data),
            portMAX_DELAY);

    }
}

void read_wheel_encoder(__unused void *params) {
    setupWheelEncoders(NULL);
}

void read_ir_sensor(__unused void *params) {
    ir_setup(NULL);
    // initHashMap(NULL); // Insert key-value pairs for barcode scanner.

    while (true) {
        vTaskDelay(10);

        read_ir(NULL);

        xMessageBufferSend(
            sendDataLeftIRSensorCMB,
            (void *) &l_ir_result,
            sizeof(l_ir_result),
            0);

        xMessageBufferSend(
            sendDataRightIRSensorCMB,
            (void *) &r_ir_result,
            sizeof(r_ir_result),
            0);

        // Troubleshooting purposes.
        // printf("Left ADC Result: %d\t Right ADC Result: %d\n", l_ir_result, r_ir_result);
        
        // For lab demo.
        // if (ir_pulse_width > 10) {
        //     if (ir_pulse_width > 1000000) {
        //         printf("Left ADC Result: %d\t Right ADC Result: %d\t Line Thickness: Thick\n", l_ir_result, r_ir_result);
        //     }
        //     else {
        //         printf("Left ADC Result: %d\t Right ADC Result: %d\t Line Thickness: Thin\n", l_ir_result, r_ir_result);
        //     }
        // }
    }
}

void read_ultrasonic_sensor(__unused void *params) {
    initUltrasonic(NULL);
    // When ultrasonic sensor detects an object, calls gpio_callback.
    // gpio_callback calls getDistanceUltrasonic to get the object's distance.
    gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO, GPIO_IRQ_EDGE_RISE, true, &gpio_callback_ultrasonic);

    while (true) {
        vTaskDelay(10);
        // Function to pulse ultrasonic sensor.
        pulseUltrasonic(NULL);
            
        // -1 means no successful pulse.
        if (final_result != -1) {
            xMessageBufferSend(
                sendDataUltrasonicSensorCMB,
                (void *) &final_result,
                sizeof(final_result),
                0);
            
            final_result = -1;
        }
    }
}

void vLaunch(void) {
    TaskHandle_t moveWheelsTask;
    xTaskCreate(move_wheels, "MoveWheelsThread", configMINIMAL_STACK_SIZE, NULL, 9, &moveWheelsTask);
    TaskHandle_t readIrSensorTask;
    xTaskCreate(read_ir_sensor, "ReadIrSensorThread", configMINIMAL_STACK_SIZE, NULL, 8, &readIrSensorTask);
    TaskHandle_t readUltrasonicSensorTask;
    xTaskCreate(read_ultrasonic_sensor, "ReadUltrasonicSensorThread", configMINIMAL_STACK_SIZE, NULL, 6, &readUltrasonicSensorTask);
    // TaskHandle_t readWheelEncoderTask;
    // xTaskCreate(read_wheel_encoder, "ReadWheelEncoderThread", configMINIMAL_STACK_SIZE, NULL, 7, &readWheelEncoderTask);

    sendDataLeftIRSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataRightIRSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataUltrasonicSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main(void)
{
    stdio_init_all();

    vLaunch();

    return 0;
}
