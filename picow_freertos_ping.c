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
// #include "hardware/magnetometer.h"

// Web server libraries.
#include "cgi.h"
#include "ssi.h"
#include "lwipopts.h"
#include "lwip/apps/httpd.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"

#define WIFI_SSID       "samuelsongg"
#define WIFI_PASSWORD   "9810423x"

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
static MessageBufferHandle_t sendDataUltrasonicSensorCMB;
static MessageBufferHandle_t sendDataLeftEncoderSpeedCMB;
static MessageBufferHandle_t sendDataRightEncoderSpeedCMB;
static MessageBufferHandle_t sendDataLeftEncoderDistanceCMB;
static MessageBufferHandle_t sendDataRightEncoderDistanceCMB;

void move_wheels(__unused void *params) {
    initMotor(NULL);
    setLeftSpeed(0.7);
    setRightSpeed(0.7);

    int left_IR_data = 0;
    int right_IR_data = 0;
    int ultrasonic_data = 0;
    double left_encoder_speed = 0.0;
    double right_encoder_speed = 0.0;
    double left_total_distance = 0.0;
    double temp_left_total_distance = 0.0;
    double right_total_distance = 0.0;
    double temp_right_total_distance = 0.0;

    while (true) {
        vTaskDelay(10);

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
            sendDataLeftEncoderSpeedCMB,
            (void *) &left_encoder_speed,
            sizeof(left_encoder_speed),
            portMAX_DELAY);

        xMessageBufferReceive(
            sendDataRightEncoderSpeedCMB,
            (void *) &right_encoder_speed,
            sizeof(right_encoder_speed),
            portMAX_DELAY);

        xMessageBufferReceive(
            sendDataLeftEncoderDistanceCMB,
            (void *) &left_total_distance,
            sizeof(left_total_distance),
            portMAX_DELAY);

        xMessageBufferReceive(
            sendDataRightEncoderDistanceCMB,
            (void *) &right_total_distance,
            sizeof(right_total_distance),
            portMAX_DELAY);

        // xMessageBufferReceive(
        //     sendDataUltrasonicSensorCMB,
        //     (void *) &ultrasonic_data,
        //     sizeof(ultrasonic_data),
        //     portMAX_DELAY);

        if (left_IR_data < COLOUR_CUTOFF_VALUE || right_IR_data < COLOUR_CUTOFF_VALUE) {
            moveForward(NULL);
            if (left_encoder_speed < right_encoder_speed || right_encoder_speed > left_encoder_speed) {
                increaseLeftSpeed(NULL);
                // decreaseRightSpeed(NULL);
            }
            if (left_encoder_speed > right_encoder_speed || right_encoder_speed < left_encoder_speed) {
                decreaseLeftSpeed(NULL);
                // increaseRightSpeed(NULL);
            }
            printf("Left Speed: %.2f\t Right Speed: %.2f\n", left_encoder_speed, right_encoder_speed);
            // printf("Moving Forward\n");
        }
        // else {
        //     temp_left_total_distance = left_total_distance + 15;
        //     temp_right_total_distance = right_total_distance + 20;

        //     if (left_total_distance < temp_left_total_distance || right_total_distance < temp_right_total_distance) {
        //         turnHardLeft(NULL);
        //     }
        //     else {
        //         stop(NULL);
        //     }
            // printf("Stopped\n");
        // }
    }
}

void read_wheel_encoder(__unused void *params) {
    setupWheelEncoders(NULL);

    while (true) {
        vTaskDelay(10);
        
        xMessageBufferSend(
                sendDataLeftEncoderSpeedCMB,
                (void *) &leftEncoderSpeed,
                sizeof(leftEncoderSpeed),
                0);

        xMessageBufferSend(
                sendDataRightEncoderSpeedCMB,
                (void *) &rightEncoderSpeed,
                sizeof(rightEncoderSpeed),
                0);

        xMessageBufferSend(
                sendDataLeftEncoderDistanceCMB,
                (void *) &leftTotalDistance,
                sizeof(leftTotalDistance),
                0);

        xMessageBufferSend(
                sendDataRightEncoderDistanceCMB,
                (void *) &rightTotalDistance,
                sizeof(rightTotalDistance),
                0);
    }
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
        //     printf("Left ADC Result: %d\t Right ADC Result: %d\t Pulse Width: %lld\n", l_ir_result, r_ir_result, ir_pulse_width);
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

            // Troubleshooting purposes.
            // printf("Ultrasonic Pulse Length: %lldcm\n", final_result);
            
            final_result = -1; // Reset to allow for next pulse.
        }
    }
}

// void read_magnetometer(__unused void *params) {
//     init_i2c(NULL);
//     init_magnetometer(NULL);
//     init_accelerometer(NULL);

//     while (true) {
//         vTaskDelay(10);
//         read_magnetometer_data(NULL);
//         // calculate_and_display_heading(x, y);
//         read_accelerometer_data(NULL);
//     }
// }

void web_server(__unused void *params) {
    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    cyw43_arch_lwip_begin();
    cyw43_arch_lwip_end();

    // Connect to the WiFI network - loop until connected
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0) {
        printf("Attempting to connect...\n");
    }
    // Print a success message once connected
    printf("Connected! \n");

    // Initialise web server
    httpd_init();
    printf("Http server initialised\n");

    // Configure SSI and CGI handler
    ssi_init();
    printf("SSI Handler initialised\n");
    cgi_init();
    printf("CGI Handler initialised\n");

    // Infinite loop
    while (true) {
        vTaskDelay(10);
    }

    cyw43_arch_deinit();
}

void vLaunch(void) {
    TaskHandle_t moveWheelsTask;
    xTaskCreate(move_wheels, "MoveWheelsThread", configMINIMAL_STACK_SIZE, NULL, 5, &moveWheelsTask);
    TaskHandle_t readIrSensorTask;
    xTaskCreate(read_ir_sensor, "ReadIrSensorThread", configMINIMAL_STACK_SIZE, NULL, 5, &readIrSensorTask);
    TaskHandle_t readUltrasonicSensorTask;
    xTaskCreate(read_ultrasonic_sensor, "ReadUltrasonicSensorThread", configMINIMAL_STACK_SIZE, NULL, 5, &readUltrasonicSensorTask);
    // TaskHandle_t readMagnetometerTask;
    // xTaskCreate(read_magnetometer, "ReadMagnetometerThread", configMINIMAL_STACK_SIZE, NULL, 5, &readMagnetometerTask);
    TaskHandle_t readWheelEncoderTask;
    xTaskCreate(read_wheel_encoder, "ReadWheelEncoderThread", configMINIMAL_STACK_SIZE, NULL, 5, &readWheelEncoderTask);
    TaskHandle_t webServerTask;
    xTaskCreate(web_server, "webserverThread", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, &webServerTask);

    sendDataLeftIRSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataRightIRSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataUltrasonicSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataLeftEncoderSpeedCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataRightEncoderSpeedCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataLeftEncoderDistanceCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataRightEncoderDistanceCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);

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
    adc_init();

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