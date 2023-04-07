#include "display.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

static int dips[2];
void dipsToDigits()
{
    char buf[10];
    int digitstoconvert = atoi(buf);
    if (digitstoconvert > 99) {
        dips[0] = 9;
        dips[1] = 9;
    }
    else {
        dips[0] = digitstoconvert / 10;
        dips[1] = digitstoconvert % 10;
    }
}

int displayInit(char *bus, int address)
{
    runCommand("config-pin p9_17 i2c");
    runCommand("config-pin p9_18 i2c");
    writeToFile(GPIO61_DIRECTION, "out");
    writeToFile(GPIO44_DIRECTION, "out");
    int i2cFileDesc = open(bus, O_RDWR);

    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

void writeI2CReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
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

void writeNumber(int num, int i2cFileDesc)
{
    writeI2CReg(i2cFileDesc, REG_DIRA, 0x00);
    writeI2CReg(i2cFileDesc, REG_DIRB, 0x00);

    switch (num) {
        case 0:
            writeI2CReg(i2cFileDesc, REG_OUTA, ZERO_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, ZERO_BOTTOM);
            break;
        case 1:
            writeI2CReg(i2cFileDesc, REG_OUTA, ONE_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, ONE_BOTTOM);
            break;
        case 2:
            writeI2CReg(i2cFileDesc, REG_OUTA, TWO_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, TWO_BOTTOM);
            break;
        case 3:
            writeI2CReg(i2cFileDesc, REG_OUTA, THREE_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, THREE_BOTTOM);
            break;
        case 4:
            writeI2CReg(i2cFileDesc, REG_OUTA, FOUR_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, FOUR_BOTTOM);
            break;
        case 5:
            writeI2CReg(i2cFileDesc, REG_OUTA, FIVE_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, FIVE_BOTTOM);
            break;
        case 6:
            writeI2CReg(i2cFileDesc, REG_OUTA, SIX_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, SIX_BOTTOM);
            break;
        case 7:
            writeI2CReg(i2cFileDesc, REG_OUTA, SEVEN_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, SEVEN_BOTTOM);
            break;
        case 8:
            writeI2CReg(i2cFileDesc, REG_OUTA, EIGHT_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, EIGHT_BOTTOM);
            break;
        case 9:
            writeI2CReg(i2cFileDesc, REG_OUTA, NINE_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, NINE_BOTTOM);
            break;
        default:
            writeI2CReg(i2cFileDesc, REG_OUTA, ZERO_TOP);
            writeI2CReg(i2cFileDesc, REG_OUTB, ZERO_BOTTOM);
            break;
    }
}

void *displayThreadFunction(void *arg)
{
    int i2cFileDesc = displayInit(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeToFile(GPIO61_VALUE, "0");
    writeToFile(GPIO44_VALUE, "0");
    while (!stopped) {
        writeNumber(dips[0], i2cFileDesc);
        writeToFile(GPIO61_VALUE, "1");
        sleepForMs(5);
        writeToFile(GPIO61_VALUE, "0");
        writeNumber(dips[1], i2cFileDesc);
        writeToFile(GPIO44_VALUE, "1");
        sleepForMs(5);
        writeToFile(GPIO44_VALUE, "0");
    }
    writeToFile(GPIO61_VALUE, "0");
    writeToFile(GPIO44_VALUE, "0");
    close(i2cFileDesc);
    return 0;
}

void *updateDigitsThread(void *arg)
{
    while (!stopped) {
        dipsToDigits();
        sleepForMs(100);
    }
    return 0;
}