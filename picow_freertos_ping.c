/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "lwip/ip4_addr.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"
#include "message_buffer.h"

#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define mbaTASK_MESSAGE_BUFFER_SIZE       ( 60 )

#ifndef PING_ADDR
#define PING_ADDR "142.251.35.196"
#endif
#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

#define TEST_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

static MessageBufferHandle_t sendDataLeftIRSensorCMB;
static MessageBufferHandle_t sendDataRightIRSensorCMB;

const uint RIGHT_WHEEL_FORWARD = 18;
const uint RIGHT_WHEEL_BACKWARD = 19;
const uint LEFT_WHEEL_FORWARD = 20;
const uint LEFT_WHEEL_BACKWARD = 21;
const uint LEFT_IR_SENSOR = 26;
const uint RIGHT_IR_SENSOR = 27;

volatile char direction = 'w';

void move_wheels(__unused void *params) {
    gpio_set_function(14, GPIO_FUNC_PWM);
    gpio_set_function(15, GPIO_FUNC_PWM);

    uint slice_num14 = pwm_gpio_to_slice_num(14);
    uint slice_num15 = pwm_gpio_to_slice_num(15);

    pwm_set_clkdiv(slice_num14, 100);
    pwm_set_clkdiv(slice_num15, 100);
    
    pwm_set_wrap(slice_num14, 12500);
    pwm_set_wrap(slice_num15, 12500);
    
    pwm_set_chan_level(slice_num14, PWM_CHAN_A, 12500 / 2);
    pwm_set_chan_level(slice_num15, PWM_CHAN_B, 12500 / 2);
    
    pwm_set_enabled(slice_num14, true);
    pwm_set_enabled(slice_num15, true);

    gpio_init(RIGHT_WHEEL_FORWARD);
    gpio_init(RIGHT_WHEEL_BACKWARD);
    gpio_init(LEFT_WHEEL_FORWARD);
    gpio_init(LEFT_WHEEL_BACKWARD);

    gpio_set_dir(RIGHT_WHEEL_FORWARD, GPIO_OUT);
    gpio_set_dir(RIGHT_WHEEL_BACKWARD, GPIO_OUT);
    gpio_set_dir(LEFT_WHEEL_FORWARD, GPIO_OUT);
    gpio_set_dir(LEFT_WHEEL_BACKWARD, GPIO_OUT);

    int left_data = 0;
    int right_data = 0;

    while (true) {
        vTaskDelay(10);

        xMessageBufferReceive(
            sendDataLeftIRSensorCMB,
            (void *) &left_data,
            sizeof(left_data),
            portMAX_DELAY);

        xMessageBufferReceive(
            sendDataRightIRSensorCMB,
            (void *) &right_data,
            sizeof(right_data),
            portMAX_DELAY);

        // Move car backwards and turn right if both sensors detect black line.
        // Insert algo here...
        if (left_data > 2000 && right_data > 2000) {
            gpio_put(RIGHT_WHEEL_FORWARD, 0);
            gpio_put(RIGHT_WHEEL_BACKWARD, 1);
            gpio_put(LEFT_WHEEL_FORWARD, 0);
            gpio_put(LEFT_WHEEL_BACKWARD, 1);

            gpio_put(RIGHT_WHEEL_FORWARD, 0);
            gpio_put(RIGHT_WHEEL_BACKWARD, 1);
            gpio_put(LEFT_WHEEL_FORWARD, 1);
            gpio_put(LEFT_WHEEL_BACKWARD, 0);
        }  
        // Move car forward if both sensors detect white space.
        else if (left_data < 2000 && right_data < 2000) {
            gpio_put(RIGHT_WHEEL_FORWARD, 1);
            gpio_put(RIGHT_WHEEL_BACKWARD, 0);
            gpio_put(LEFT_WHEEL_FORWARD, 1);
            gpio_put(LEFT_WHEEL_BACKWARD, 0);  
        }
        // Turn car right is left sensor detects black line.
        else if (left_data > 2000 && right_data < 2000) {
            gpio_put(RIGHT_WHEEL_FORWARD, 0);
            gpio_put(RIGHT_WHEEL_BACKWARD, 1);
            gpio_put(LEFT_WHEEL_FORWARD, 1);
            gpio_put(LEFT_WHEEL_BACKWARD, 0);
        }
        // Turn car left if right sensor detects black line.
        else if (right_data > 2000 && left_data < 2000) {
            gpio_put(RIGHT_WHEEL_FORWARD, 1);
            gpio_put(RIGHT_WHEEL_BACKWARD, 0);
            gpio_put(LEFT_WHEEL_FORWARD, 0);
            gpio_put(LEFT_WHEEL_BACKWARD, 1);
        } 
    }
}

void get_direction(__unused void *params) {
    while (true) {
        char input_direction = getchar();

        if (input_direction == 'w') {
            direction = 'w';
            printf("Direction: %c\n", direction);
        }
        else if (input_direction == 's') {
            direction = 's';
            printf("Direction: %c\n", direction);
        }
    }
}

void read_ir_sensor(__unused void *params) {
    adc_init();
    adc_set_temp_sensor_enabled(true);

    gpio_init(2); // LEFT_IR_Sensor's VCC
    gpio_init(3); // LEFT_IR_Sensor's GND
    gpio_init(4); // RIGHT_IR_Sensor's VCC
    gpio_init(5); // RIGHT_IR_Sensor's GND

    gpio_set_dir(2, GPIO_OUT);
    gpio_set_dir(3, GPIO_OUT);
    gpio_set_dir(4, GPIO_OUT);
    gpio_set_dir(5, GPIO_OUT);

    gpio_put(2, 1); // Set to high for LEFT_IR_Sensor's VCC
    gpio_put(3, 0); // Set to low for LEFT_IR_Sensor's GND
    gpio_put(4, 1); // Set to high for RIGHT_IR_Sensor's VCC
    gpio_put(5, 0); // Set to low for RIGHT_IR_Sensor's GND

    gpio_set_dir(LEFT_IR_SENSOR, GPIO_IN); // IR Sensor
    gpio_set_function(LEFT_IR_SENSOR, GPIO_FUNC_SIO);
    gpio_disable_pulls(LEFT_IR_SENSOR);
    gpio_set_input_enabled(LEFT_IR_SENSOR, false);
    
    gpio_set_dir(RIGHT_IR_SENSOR, GPIO_IN); // IR Sensor
    gpio_set_function(RIGHT_IR_SENSOR, GPIO_FUNC_SIO);
    gpio_disable_pulls(RIGHT_IR_SENSOR);
    gpio_set_input_enabled(RIGHT_IR_SENSOR, false);

    int l_sum = 0;
    static int l_data[10] = {0};
    static int l_count = 0;
    static int l_index = 0;

    int r_sum = 0;
    static int r_data[10] = {0};
    static int r_count = 0;
    static int r_index = 0;

    while (true) {
        vTaskDelay(10);

        adc_select_input(0);
        uint32_t l_result = adc_read();
        adc_select_input(1);
        uint32_t r_result = adc_read();
        
        // Calculate moving average of 10 data points.
        l_sum -= l_data[l_index];
        l_data[l_index] = l_result;
        l_sum += l_data[l_index];
        l_index = (l_index + 1) % 10;
        
        r_sum -= r_data[r_index];
        r_data[r_index] = r_result;
        r_sum += r_data[r_index];
        r_index = (r_index + 1) % 10;
        
        if (l_count < 10) l_count++;
        if (r_count < 10) r_count++;

        l_result = l_sum / l_count;
        r_result = r_sum / r_count;

        xMessageBufferSend(
            sendDataLeftIRSensorCMB,
            (void *) &l_result,
            sizeof(l_result),
            0);

        xMessageBufferSend(
            sendDataRightIRSensorCMB,
            (void *) &r_result,
            sizeof(r_result),
            0);

        printf("Left ADC Result: %d\n", l_result);
        printf("Right ADC Result: %d\n", r_result);
    }
}

void vLaunch(void) {
    // TaskHandle_t getDirectionTask;
    // xTaskCreate(get_direction, "GetDirectionThread", configMINIMAL_STACK_SIZE, NULL, 9, &getDirectionTask);
    TaskHandle_t moveWheelsTask;
    xTaskCreate(move_wheels, "MoveWheelsThread", configMINIMAL_STACK_SIZE, NULL, 7, &moveWheelsTask);
    TaskHandle_t readIrSensorTask;
    xTaskCreate(read_ir_sensor, "ReadIrSensorThread", configMINIMAL_STACK_SIZE, NULL, 8, &readIrSensorTask);

    // xControlMessageBuffer = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataLeftIRSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataRightIRSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
    // takes care of it otherwise)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main(void)
{
    stdio_init_all();

    /* Configure the hardware ready to run the demo. */
    const char *rtos_name;
#if ( portSUPPORT_SMP == 1 )
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if ( portSUPPORT_SMP == 1 ) && ( configNUM_CORES == 2 )
    printf("Starting %s on both cores:\n", rtos_name);
    vLaunch();
#elif ( RUN_FREERTOS_ON_CORE == 1 )
    printf("Starting %s on core 1:\n", rtos_name);
    multicore_launch_core1(vLaunch);
    while (true);
#else
    printf("Starting %s on core 0:\n", rtos_name);
    vLaunch();
#endif
    return 0;
}

/*** end of file ***/