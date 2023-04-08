// header for handling accelerometer
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define OUT_X_MSB 0x01
#define OUT_X_LSB 0x02
#define OUT_Y_MSB 0x03
#define OUT_Y_LSB 0x04
#define OUT_Z_MSB 0x05
#define OUT_Z_LSB 0x06
#define ACTIVE_MODE 0x2a

#define G 1024
#define HYSTERESIS 0.1*G

#define ACCELEROMETER_BUS "/dev/i2c-1"
#define I2C_ADDRESS 0x1c

enum {
    NONE, LEFT_UP, LEFT, LEFT_DOWN, CENTER, CENTER_UP, CENTER_DOWN, RIGHT_UP, RIGHT, RIGHT_DOWN, LEFT_FAR_UP, LEFT_FAR_DOWN, RIGHT_FAR_UP, RIGHT_FAR_DOWN, CENTER_FAR_UP, CENTER_FAR_DOWN
};

extern int currentState;

// Initialize the accelerometer
void initAccelerometer();

// Write a value to an i2c register
void writeI2CReg(unsigned char regAddr, unsigned char value);

// Initialize the i2c bus
int initI2cBus(char* bus, int address);

// Read a register from the accelerometer
unsigned char readI2cReg(unsigned char regAddr);

// Determine the state of the accelerometer
void determineState(int *r_point);

// Thread to read the accelerometer
void* accelerometerThread(void* arg);

// Reset the random point
void resetRandomPoint();

// Get the accelerometer values
int* getAccel();

// Get a random point from -0.5G to 0.5G
int *randomPoint();

#endif
