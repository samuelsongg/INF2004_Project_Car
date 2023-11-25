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

#define WIFI_SSID       "SSID"
#define WIFI_PASSWORD   "PASSWORD"

#define mbaTASK_MESSAGE_BUFFER_SIZE       ( 60 )

static MessageBufferHandle_t sendDataLeftIRSensorCMB;
static MessageBufferHandle_t sendDataRightIRSensorCMB;
static MessageBufferHandle_t sendDataUltrasonicSensorCMB;
static MessageBufferHandle_t sendDataLeftEncoderCMB;
static MessageBufferHandle_t sendDataRightEncoderCMB;

volatile int temp_left_notch_count = 0;
volatile int temp_right_notch_count = 0;

void move_wheels(__unused void *params) {
    initMotor(NULL);

    int left_IR_data = 0;
    int right_IR_data = 0;
    int ultrasonic_data = 0;
    double left_encoder_speed = 0.0;
    double right_encoder_speed = 0.0;

    setLeftSpeed(0.5);
    setRightSpeed(0.5);

    while (true) {
        // If both IR sensors detect white space, move forward.
        if (left_IR_data < COLOUR_CUTOFF_VALUE && right_IR_data < COLOUR_CUTOFF_VALUE) {
            setLeftSpeed(0.53);
            setRightSpeed(0.5);
            
            moveForward(NULL);

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

            temp_left_notch_count = leftNotchCount;
            temp_right_notch_count = rightNotchCount;

            while ((temp_left_notch_count > (leftNotchCount - 25))) {
                turnHardRight(NULL);
            }
        }
        else if(left_IR_data > COLOUR_CUTOFF_VALUE) {
            setLeftSpeed(0.2);
            setRightSpeed(1);
        }
        else if(right_IR_data > COLOUR_CUTOFF_VALUE) {
            setLeftSpeed(1);
            setRightSpeed(0.2);
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

void read_ir_sensor(__unused void *params) {
    ir_setup(NULL);

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
    }
}

void read_barcode(__unused void *params) {
    barcode_setup();

    while (true) {
        vTaskDelay(10);

        barcode_main_loop();
    }
}

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == LEFT_ENCODER_PIN) {
        leftEncoder(NULL);
    }

    if (gpio == RIGHT_ENCODER_PIN) {
        rightEncoder(NULL);
    }

    if (gpio == ULTRASONIC_ECHO) {
        getDistanceUltrasonic(NULL);
    }
}

void read_ultrasonic_sensor(__unused void *params) {
    initUltrasonic(NULL);

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

void read_magnetometer_task(__unused void *params) {
    setup_magnetometer(NULL);

    while (true) {
        vTaskDelay(10);

        read_magnetometer(NULL);
    }
}

void interrupt_task(__unused void *params) {
    gpio_set_irq_enabled_with_callback(LEFT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(RIGHT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    while (true) {
        vTaskDelay(10);
    }
}

void web_server_task(__unused void *params) {
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
    // TaskHandle_t magnetometerTask;
    // xTaskCreate(read_magnetometer_task, "MagnetometerThread", configMINIMAL_STACK_SIZE, NULL, 5, &magnetometerTask);

    sendDataLeftIRSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataRightIRSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataUltrasonicSensorCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataLeftEncoderCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataRightEncoderCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main(void)
{
    stdio_init_all();
    sleep_ms(3000);
    adc_init();
    vLaunch();

    return 0;
}

/*** End of file ***/