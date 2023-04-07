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

#define ACCELEROMETER_BUS "/dev/i2c-1"
#define I2C_ADDRESS 0x1c

// Initialize the accelerometer
void initAccelerometer();

// Write a value to an i2c register
void writeI2CReg(unsigned char regAddr, unsigned char value);

// Initialize the i2c bus
int initI2cBus(char* bus, int address);

// Read a register from the accelerometer
unsigned char readI2cReg(unsigned char regAddr);

// Thread to read the accelerometer
void* accelerometerThread(void* arg);

// Get the accelerometer values
int* getAccel();

// Get the largest change in the accelerometer
char getLargestChange();

#endif
