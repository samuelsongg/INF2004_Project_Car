#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
//

// Constants for I2C and sensor addresses
#define MAGNETOMETER_ADDRESS 0x1E  // I2C address for GY-511 magnetometer
#define GY511_CONFIG_A 0x00        // Configuration Register A for magnetometer
#define GY511_DATA 0x03            // Data Output Register for magnetometer
#define ACCELEROMETER_ADDRESS 0x19 // I2C address for the accelerometer

// Function to initialize I2C communication
void init_i2c() {
    i2c_init(i2c0, 400000);          // Initialize I2C with a 400 kHz baud rate
    gpio_set_function(0, GPIO_FUNC_I2C);  // Set GPIO0 as SDA
    gpio_set_function(1, GPIO_FUNC_I2C);  // Set GPIO1 as SCL
    gpio_pull_up(0);                  // Enable pull-up on SDA
    gpio_pull_up(1);                  // Enable pull-up on SCL
}

// Function to initialize the accelerometer
void init_accelerometer() {
    // Configuration register addresses and values for the accelerometer setup
    const uint8_t CTRL_REG1_A = 0x20;
    const uint8_t ENABLE_ACCEL = 0x57;  // 100 Hz data rate, all axes enabled, normal mode
    const uint8_t CTRL_REG4_A = 0x23;
    const uint8_t FULL_SCALE = 0x00;    // +/- 2g scale

    // Activate the accelerometer by writing to control register 1
    uint8_t config[] = {CTRL_REG1_A, ENABLE_ACCEL};
    i2c_write_blocking(i2c0, ACCELEROMETER_ADDRESS, config, sizeof(config), true);

    // Additional settings for the accelerometer (e.g., full-scale range)
    config[0] = CTRL_REG4_A;
    config[1] = FULL_SCALE;
    i2c_write_blocking(i2c0, ACCELEROMETER_ADDRESS, config, sizeof(config), true);
}

// Function to read data from the accelerometer
void read_accelerometer_data() {
    // Address for the accelerometer data registers with auto-increment bit set
    const uint8_t OUT_X_L_A = 0x28 | 0x80;
    uint8_t accel_data[6] = {0};  // Buffer to hold the raw accelerometer data

    // Request accelerometer data
    i2c_write_blocking(i2c0, ACCELEROMETER_ADDRESS, &OUT_X_L_A, 1, true);
    i2c_read_blocking(i2c0, ACCELEROMETER_ADDRESS, accel_data, sizeof(accel_data), true);

    // Combine the high and low bytes for each axis and convert to meaningful values
    int16_t x_acc = (int16_t)((accel_data[1] << 8) | accel_data[0]) >> 4;
    int16_t y_acc = (int16_t)((accel_data[3] << 8) | accel_data[2]) >> 4;
    int16_t z_acc = (int16_t)((accel_data[5] << 8) | accel_data[4]) >> 4;

    //printf("X: %.4f, Y: %.4f, Z: %.4f\n", x_acc, y_acc, z_acc);
    printf("heading angle:", )
}

// Function to initialize the magnetometer
void init_magnetometer() {
    // Set the magnetometer to continuous measurement mode with default configuration
    // Addresses and values for the magnetometer control registers
    const uint8_t MR_REG_M = 0x02;
    const uint8_t CONTINUOUS_CONVERSION = 0x00;
    const uint8_t CRA_REG_M = 0x00;
    const uint8_t DATA_RATE = 0x10;  // 15Hz data rate
    const uint8_t CRB_REG_M = 0x01;
    const uint8_t GAIN = 0x20;       // +/- 1.3g scale

    // Enable continuous conversion mode on the magnetometer
    uint8_t config[] = {MR_REG_M, CONTINUOUS_CONVERSION};
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, config, sizeof(config), true);

    // Set the data rate on the magnetometer
    config[0] = CRA_REG_M;
    config[1] = DATA_RATE;
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, config, sizeof(config), true);

    // Set the gain on the magnetometer
    config[0] = CRB_REG_M;
    config[1] = GAIN;
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, config, sizeof(config), true);
}

// Function to read data from the magnetometer
void read_magnetometer_data() {
    uint8_t reg[1] = {0};
    uint8_t data[1] = {0};

    // Read 6 bytes of data from the magnetometer
    // MSB first
    // Read xMag MSB data from register(0x03)
    reg[0] = 0x03;
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, reg, sizeof(reg), true);
    i2c_read_blocking(i2c0, MAGNETOMETER_ADDRESS, data, sizeof(data), true);
    uint8_t data1_0 = data[0];

    // Read xMag lsb data from register(0x04)
    reg[0] = 0x04;
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, reg, sizeof(reg), true);
    i2c_read_blocking(i2c0, MAGNETOMETER_ADDRESS, data, sizeof(data), true);
    uint8_t data1_1 = data[0];

    // Read yMag msb data from register(0x05)
    reg[0] = 0x07;
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, reg, sizeof(reg), true);
    i2c_read_blocking(i2c0, MAGNETOMETER_ADDRESS, data, sizeof(data), true);
    uint8_t data1_2 = data[0];

    // Read yMag lsb data from register(0x06)
    reg[0] = 0x08;
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, reg, sizeof(reg), true);
    i2c_read_blocking(i2c0, MAGNETOMETER_ADDRESS, data, sizeof(data), true);
    uint8_t data1_3 = data[0];

    // Read zMag msb data from register(0x07)
    reg[0] = 0x05;
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, reg, sizeof(reg), true);
    i2c_read_blocking(i2c0, MAGNETOMETER_ADDRESS, data, sizeof(data), true);
    uint8_t data1_4 = data[0];

    // Read zMag lsb data from register(0x08)
    reg[0] = 0x06;
    i2c_write_blocking(i2c0, MAGNETOMETER_ADDRESS, reg, sizeof(reg), true);
    i2c_read_blocking(i2c0, MAGNETOMETER_ADDRESS, data, sizeof(data), true);
    uint8_t data1_5 = data[0];

    // Convert the data to meaningful values
    int16_t x = (data1_0 << 8) | data1_1;
    int16_t y = (data1_2 << 8) | data1_3;
    int16_t z = (data1_4 << 8) | data1_5;
    // (Similar conversions for other axes...)

    printf("X: %.4f Gauss, Y: %.4f Gauss, Z: %.4f Gauss\n", x / 1370.0, y / 1370.0, z / 1370.0);
}

// Function to calculate and display the heading angle
void calculate_and_display_heading(int16_t x, int16 y) {
    // Calculate the heading angle (in degrees) using the arctan2 function
    double heading_rad = atan2(y, x);
    double heading_deg = heading_rad * (180.0 / M_PI);

    // Ensure the angle is between 0 and 360 degrees
    if (heading_deg < 0) {
        heading_deg += 360.0;
    }

    printf("Heading: %.2f degrees\n", heading_deg);

}

int main() {
    stdio_init_all();
    init_i2c();
    init_magnetometer();
    init_accelerometer();

    while (1) {
        printf("--------------------\n");
        read_magnetometer_data();
        calculate_and_display_heading(x, y);
        read_accelerometer_data();
        sleep_ms(500);  // Sleep for 500 milliseconds
    }

    return 0;
}
