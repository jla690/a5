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

int initI2cBus(char* bus, int address)
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

void* accelerometerThread(void* arg)
{
    initAccelerometer();
    while (!stopped) {
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
        // printf("x: %d, y: %d, z: %d\n", x, y, z);
        sleepForMs(10);
    }
    close(i2cFileDesc);
    return 0;
}

int* getAccel()
{
    static int accel[3];
    pthread_mutex_lock(&lock);
    accel[0] = x;
    accel[1] = y;
    accel[2] = z;
    pthread_mutex_unlock(&lock);
    return accel;
}

char getLargestChange() {
    int* accel = getAccel();
    int temp_x = accel[0];
    int temp_y = accel[1];
    int temp_z = accel[2];
    temp_z -= (G - 100);
    if (abs(temp_x) > abs(temp_y) && abs(temp_x) > abs(temp_z) && abs(temp_x) > G) {
        return 'x';
    } else if (abs(temp_y) > abs(temp_x) && abs(temp_y) > abs(temp_z) && abs(temp_y) > G) {
        return 'y';
    } else if (abs(temp_z) > abs(temp_x) && abs(temp_z) > abs(temp_y) && abs(temp_z) > G) {
        return 'z';
    }
    return 'n';
}
