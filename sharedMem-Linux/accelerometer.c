#include "accelerometer.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "utils.h"

int i2cFileDesc = 0;

static int16_t x = 0;
static int16_t y = 0;
static int16_t z = 0;

int reset = 0;

int currentState = NONE;

void initAccelerometer()
{
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
    i2cFileDesc = initI2cBus(ACCELEROMETER_BUS, I2C_ADDRESS);
    writeI2CReg(ACTIVE_MODE, 0x01);
}

void writeI2CReg(unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write to I2C register.");
        exit(1);
    }
}

int initI2cBus(char *bus, int address)
{
    i2cFileDesc = open(bus, O_RDWR);
    if (i2cFileDesc < 0) {
        perror("I2C: Unable to open i2c device.");
        exit(1);
    }
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

unsigned char readI2cReg(unsigned char regAddr)
{
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));
    if (res != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
}

void determineState(int* r_point) {
    if (x > r_point[0] + HYSTERESIS) {
        if (y > r_point[1] + HYSTERESIS) {
            currentState = LEFT_UP;
            if (y > G - HYSTERESIS) {
                currentState = LEFT_FAR_UP;
            }
        } else if (y < r_point[1] - HYSTERESIS) {
            currentState = LEFT_DOWN;
            if (y < -G + HYSTERESIS) {
                currentState = LEFT_FAR_DOWN;
            }
        } else {
            currentState = LEFT;
        }
    }
    else if (x < r_point[0] - HYSTERESIS) {
        if (y > r_point[1] + HYSTERESIS) {
            currentState = RIGHT_UP;
            if (y > G - HYSTERESIS) {
                currentState = RIGHT_FAR_UP;
            }
        } else if (y < r_point[1] - HYSTERESIS) {
            currentState = RIGHT_DOWN;
            if (y < -G + HYSTERESIS) {
                currentState = RIGHT_FAR_DOWN;
            }
        } else {
            currentState = RIGHT;
        }
    }
    else {
        if (y > r_point[1] + HYSTERESIS) {
            currentState = CENTER_UP;
            if (y > G - HYSTERESIS) {
                currentState = CENTER_FAR_UP;
            }
        } else if (y < r_point[1] - HYSTERESIS) {
            currentState = CENTER_DOWN;
            if (y < -G + HYSTERESIS) {
                currentState = CENTER_FAR_DOWN;
            }
        } else {
            currentState = CENTER;
        }
    }
}

void *accelerometerThread(void *arg)
{
    initAccelerometer();
    while (!stopped) {
        int *r_point = randomPoint();
        reset = 0;
        while (!reset) {
            char buffer[7];
            if (read(i2cFileDesc, buffer, sizeof(buffer)) != 7) {
                printf("Error reading I2C\n");
                exit(1);
            }
            x = (buffer[OUT_X_MSB] << 8) | (buffer[OUT_X_LSB]);
            y = (buffer[OUT_Y_MSB] << 8) | (buffer[OUT_Y_LSB]);
            z = (buffer[OUT_Z_MSB] << 8) | (buffer[OUT_Z_LSB]);
            x /= 16;
            y /= 16;
            z /= 16;
            printf("Random Point: %d, %d, %d\n", r_point[0], r_point[1],
                   r_point[2]);
            printf("Actual: X: %d, Y: %d, Z: %d\n", x, y, z);
            determineState(r_point);
            sleepForMs(100);
        }
    }
    close(i2cFileDesc);
    return 0;
}

void resetRandomPoint() {
    reset = 1;
}

int *getAccel()
{
    static int accel[3];
    pthread_mutex_lock(&lock);
    accel[0] = x;
    accel[1] = y;
    accel[2] = z;
    pthread_mutex_unlock(&lock);
    return accel;
}

int *randomPoint()
{
    srand((unsigned)time(NULL));
    static int point[3];
    point[0] = (rand() % G) - (G / 2);
    point[1] = (rand() % G) - (G / 2);
    point[2] = 0;
    return point;
}