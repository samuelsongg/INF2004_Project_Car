/**
 * @file picow_freetos_ping.c
 * @brief Main file for the car using FreeRTOS.
 *
 * This file contains the main function and task definitions for controlling the car.
 * It includes initializations for various hardware components and tasks for sensor
 * reading, wheel movement, web server operation, and other functionalities.
 */

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

// Web Server
#include "cgi.h"
#include "ssi.h"
#include "lwipopts.h"
#include "lwip/apps/httpd.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"

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
#include "hardware/magnetometer.h"
#include "hardware/barcode.h"

// Wifi Configuration
#define WIFI_SSID       "SSID"
#define WIFI_PASSWORD   "PASSWORD"

#define mbaTASK_MESSAGE_BUFFER_SIZE       ( 60 )

/**
 * @brief Task to control wheel movement based on sensor data.
 *
 * @param params Task parameters
 */
void move_wheels(__unused void *params) {
    // Initialize motor control
    initMotor(NULL);

    setLeftSpeed(0.5);
    setRightSpeed(0.5);

    while (true) {
        // Read IR sensor data
        int left_IR_data = getLeftIRSensorValue(NULL);
        int right_IR_data = getRightIRSensorValue(NULL);

        // If both IR sensors detect white space, move forward.
        if (getUltrasonicFinalResult(NULL) < 15) {
            stop(NULL);
        }
        else {
            // If both IRs detect white, move forward.
            if (left_IR_data < COLOUR_CUTOFF_VALUE && right_IR_data < COLOUR_CUTOFF_VALUE) {
                setLeftSpeed(0.53);
                setRightSpeed(0.5);
                
                moveForward(NULL);

                double left_encoder_speed = getLeftSpeed(NULL);
                double right_encoder_speed = getRightSpeed(NULL);

                // Make sure both wheels are moving at the same speed.
                if (left_encoder_speed < right_encoder_speed + 0.6) {
                    increaseLeftSpeed(NULL);
                }
                if (left_encoder_speed > right_encoder_speed + 0.6) {
                    decreaseLeftSpeed(NULL);
                }
            }
            // If both IR sensors detect black line, turn.
            else if (left_IR_data > COLOUR_CUTOFF_VALUE && right_IR_data > COLOUR_CUTOFF_VALUE) {
                setLeftSpeed(0.5);
                setRightSpeed(0.5);

                int temp_left_notch_count = getLeftNotchCount(NULL);

                // Turn right until the left wheel has turned 25 notches.
                while ((temp_left_notch_count > (getLeftNotchCount(NULL) - 25))) {
                    turnHardRight(NULL);
                }
            }
            // If left IR detects black, angle right.
            else if(left_IR_data > COLOUR_CUTOFF_VALUE) {
                setLeftSpeed(0.2);
                setRightSpeed(1);
            }
            // If right IR detects black, angle left.
            else if(right_IR_data > COLOUR_CUTOFF_VALUE) {
                setLeftSpeed(1);
                setRightSpeed(0.2);
            }
        }
    }
}

/**
 * @brief Task to read IR sensor data at regular intervals.
 *
 * @param params Task parameters
 */
void read_ir_sensor(__unused void *params) {
    // Setup IR sensor
    ir_setup(NULL);

    while (true) {
        // Read IR sensor at regular intervals
        vTaskDelay(10);
        read_ir(NULL);
    }
}

/**
 * @brief Task to continuously read barcode data.
 *
 * @param params Task parameters
 */
void read_barcode(__unused void *params) {
    // Setup barcode reading
    barcode_setup();

    while (true) {
        vTaskDelay(10);
        // Function to read barcode
        barcode_main_loop();
    }
}

/**
 * @brief GPIO callback function to handle encoder and ultrasonic sensor inputs.
 *
 * @param gpio GPIO pin number.
 * @param events Event type that triggered the callback.
 */
void gpio_callback(uint gpio, uint32_t events) {
    // Handle encoder inputs
    if (gpio == LEFT_ENCODER_PIN) {
        leftEncoder(NULL);
    }

    if (gpio == RIGHT_ENCODER_PIN) {
        rightEncoder(NULL);
    }

    // Handle ultrasonic sensor echo
    if (gpio == ULTRASONIC_ECHO) {
        getDistanceUltrasonic(NULL);
    }
}

/**
 * @brief Task to read ultrasonic sensor data at regular intervals.
 *
 * @param params Task parameters
 */
void read_ultrasonic_sensor(__unused void *params) {
    // Initialize ultrasonic sensor
    initUltrasonic(NULL);

    while (true) {
        vTaskDelay(10);
        // Function to pulse ultrasonic sensor.
        pulseUltrasonic(NULL);

        int ultrasonic_distance = getUltrasonicFinalResult(NULL);
            
        // -1 means no successful pulse. Reset the distance to -1.
        if (ultrasonic_distance != -1) {
            ultrasonic_distance = -1;
        }
    }
}

/**
 * @brief Task to continuously read magnetometer data.
 *
 * @param params Task parameters
 */
void read_magnetometer_task(__unused void *params) {
    // Setup magnetometer
    setup_magnetometer(NULL);

    while (true) {
        vTaskDelay(10);
        // Function to read magnetometer
        read_magnetometer(NULL);
    }
}

/**
 * @brief Task to set up and manage GPIO interrupts.
 *
 * @param params Task parameters (unused).
 */
void interrupt_task(__unused void *params) {
    // Setup GPIO interrupts
    gpio_set_irq_enabled_with_callback(LEFT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(RIGHT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    while (true) {
        vTaskDelay(10);
    }
}

/**
 * @brief Task to initialize and run the web server.
 *
 * @param params Task parameters (unused).
 */
void web_server_task(__unused void *params) {
    // Setup Webserver
    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    cyw43_arch_lwip_begin();
    cyw43_arch_lwip_end();

    // Connect to the WiFI network - loop until connected
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0)
    {
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
    while (true)
    {
        vTaskDelay(10);
    }

    cyw43_arch_deinit();
}

/**
 * @brief Launches all the FreeRTOS tasks.
 */
void vLaunch(void) {
    TaskHandle_t webServerTask;
    xTaskCreate(web_server_task, "webserverThread", configMINIMAL_STACK_SIZE, NULL, 2, &webServerTask);
    TaskHandle_t moveWheelsTask;
    xTaskCreate(move_wheels, "MoveWheelsThread", configMINIMAL_STACK_SIZE, NULL, 2, &moveWheelsTask);
    TaskHandle_t readIrSensorTask;
    xTaskCreate(read_ir_sensor, "ReadIrSensorThread", configMINIMAL_STACK_SIZE, NULL, 2, &readIrSensorTask);
    TaskHandle_t readUltrasonicSensorTask;
    xTaskCreate(read_ultrasonic_sensor, "ReadUltrasonicSensorThread", configMINIMAL_STACK_SIZE, NULL, 2, &readUltrasonicSensorTask);
    TaskHandle_t interruptTask;
    xTaskCreate(interrupt_task, "InterruptThread", configMINIMAL_STACK_SIZE, NULL, 2, &interruptTask);
    TaskHandle_t readBarcodeTask;
    xTaskCreate(read_barcode, "ReadBarcodeThread", configMINIMAL_STACK_SIZE, NULL, 2, &readBarcodeTask);
    TaskHandle_t magnetometerTask;
    xTaskCreate(read_magnetometer_task, "MagnetometerThread", configMINIMAL_STACK_SIZE, NULL, 5, &magnetometerTask);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

/**
 * @brief Main function to initialize the system and start the scheduler.
 *
 * @return Execution status code (should not return under normal operation).
 */
int main(void)
{
    stdio_init_all();
    sleep_ms(3000);
    adc_init();
    vLaunch();

    return 0;
}

/*** End of file ***/