#ifndef _MAGNETOMETER_H
#define _MAGNETOMETER_H

// Constants for I2C and sensor addresses
#define MAGNETOMETER_ADDRESS 0x1E  // I2C address for GY-511 magnetometer
#define GY511_CONFIG_A 0x00        // Configuration Register A for magnetometer
#define GY511_DATA 0x03            // Data Output Register for magnetometer
#define ACCELEROMETER_ADDRESS 0x19 // I2C address for the accelerometer

// Function declarations
void init_i2c();
void init_accelerometer();
void read_accelerometer_data();
void init_magnetometer();
double read_magnetometer_data();
void resetDirection();
void setup();

#endif

/*** End of file ***/