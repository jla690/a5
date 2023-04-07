// Header file for handling displaying digits
#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define GPIO44_DIRECTION "/sys/class/gpio/gpio44/direction"
#define GPIO61_DIRECTION "/sys/class/gpio/gpio61/direction"

#define GPIO44_VALUE "/sys/class/gpio/gpio44/value"
#define GPIO61_VALUE "/sys/class/gpio/gpio61/value"

#define I2C_DEVICE_ADDRESS 0x20

// register addresses for I2C lights
#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

// patterns for digits
#define ZERO_TOP 0xA1
#define ZERO_BOTTOM 0x86
#define ONE_TOP 0x80
#define ONE_BOTTOM 0x12
#define TWO_TOP 0x31
#define TWO_BOTTOM 0x0E
#define THREE_TOP 0xB0
#define THREE_BOTTOM 0x0E
#define FOUR_TOP 0x90
#define FOUR_BOTTOM 0x8A
#define FIVE_TOP 0xB0
#define FIVE_BOTTOM 0x8C
#define SIX_TOP 0xB1
#define SIX_BOTTOM 0x8C
#define SEVEN_TOP 0x80
#define SEVEN_BOTTOM 0x06
#define EIGHT_TOP 0xB1
#define EIGHT_BOTTOM 0x8E
#define NINE_TOP 0x90
#define NINE_BOTTOM 0x8E

// initialize the display
int displayInit(char* bus, int address);

// write to the I2C register
void writeI2CReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);

// thread for displaying the number
void* displayThreadFunction(void* arg);

// thread for updating the digits
void* updateDigitsThread(void* arg);

// writing certain numbers to the display
void writeNumber(int num, int i2cFileDesc);

// converting int to left and right digits
void dipsToDigits();
