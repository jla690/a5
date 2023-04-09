#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "accelerometer.h"
#include "sharedDataStruct.h"
#include "sharedMem-Linux.h"
#include "thread_control.h"
#include "utils.h"

// 0x0f000000,     // Green
// 0x000f0000, // Red
// 0x00000f00, // Blue
// 0x0000000f, // White
// 0x0f0f0f00, // White (via RGB)
// 0x0f0f0000, // Yellow
// 0x000f0f00, // Purple
// 0x0f000f00, // Teal

// Try these; they are birght!
// (You'll need to comment out some of the above)
// 0xff000000, // Green Bright
// 0x00ff0000, // Red Bright
// 0x0000ff00, // Blue Bright
// 0xffffff00, // White
// 0xff0000ff, // Green Bright w/ Bright White
// 0x00ff00ff, // Red Bright w/ Bright White
// 0x0000ffff, // Blue Bright w/ Bright White
// 0xffffffff, // White w/ Bright White

// FOR GPIO PINS TO BE CONTROLLED BY PRU0
// config-pin p8_12 pruout
// config-pin p8_15 pruin

// NEOPIXEL
// NEED THIS FOR NEOPIXEL TO RUN
// config-pin P8.11 pruout

// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------
#define PRU_ADDR 0x4A300000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000     // Length of PRU memory
#define PRU0_DRAM 0x00000   // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000  // Offset to shared memory
#define PRU_MEM_RESERVED 0x200 // Amount used by stack and heap

#define LED_LEN 8

#define GREEN 0x0f000000
#define RED 0x000f0000
#define BLUE 0x00000f00
#define BRIGHT_RED 0x00ff0000
#define BRIGHT_GREEN 0xff000000
#define BRIGHT_BLUE 0x0000ff00

// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ((base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ((base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ((base) + PRU_SHAREDMEM)

// Return the address of the PRU's base memory
volatile void *getPruMmapAddr(void)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1)
    {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }

    // Points to start of PRU memory.
    volatile void *pPruBase =
        mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED)
    {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pPruBase;
}

void turnOnBottomLEDs(volatile sharedMemStruct_t *pSharedPru0, int color)
{
    for (int i = 0; i < 3; i++)
    {
        if (i == 1)
        {
            switch (color)
            {
            case RED:
                pSharedPru0->LEDS[i] = BRIGHT_RED;
                break;
            case GREEN:
                pSharedPru0->LEDS[i] = BRIGHT_GREEN;
                break;
            case BLUE:
                pSharedPru0->LEDS[i] = BRIGHT_BLUE;
                break;
            }
        }
        else
        {
            pSharedPru0->LEDS[i] = color;
        }
    }
    for (int i = 3; i < LED_LEN; i++)
    {
        pSharedPru0->LEDS[i] = 0;
    }
}

void setAllLEDs(volatile sharedMemStruct_t *pSharedPru0, int color)
{
    for (int i = 0; i < LED_LEN; i++)
    {
        pSharedPru0->LEDS[i] = color;
    }
}

void turnOnTopLEDs(volatile sharedMemStruct_t *pSharedPru0, int color)
{
    for (int i = 0; i < 5; i++)
    {
        pSharedPru0->LEDS[i] = 0;
    }
    for (int i = 5; i < LED_LEN; i++)
    {
        if (i == 6)
        {
            switch (color)
            {
            case RED:
                pSharedPru0->LEDS[i] = BRIGHT_RED;
                break;
            case GREEN:
                pSharedPru0->LEDS[i] = BRIGHT_GREEN;
                break;
            case BLUE:
                pSharedPru0->LEDS[i] = BRIGHT_BLUE;
                break;
            }
        }
        else
        {
            pSharedPru0->LEDS[i] = color;
        }
    }
}

void initializeLEDs() { runCommand("config-pin P8.11 pruout"); }

void *LEDThread(void *arg)
{
    volatile void *pPruBase = getPruMmapAddr();
    volatile sharedMemStruct_t *pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);
    while (!stopped)
    {
        switch (currentState)
        {
        case NONE:
            break;
        case LEFT_UP:
            turnOnTopLEDs(pSharedPru0, RED);
            break;
        case LEFT_FAR_UP:
            setAllLEDs(pSharedPru0, 0);
            pSharedPru0->LEDS[LED_LEN - 1] = BRIGHT_RED;
            break;
        case LEFT_DOWN:
            turnOnBottomLEDs(pSharedPru0, RED);
            break;
        case LEFT_FAR_DOWN:
            setAllLEDs(pSharedPru0, 0);
            pSharedPru0->LEDS[0] = BRIGHT_RED;
            break;
        case LEFT:
            setAllLEDs(pSharedPru0, RED);
            break;
        case RIGHT_UP:
            turnOnTopLEDs(pSharedPru0, GREEN);
            break;
        case RIGHT_FAR_UP:
            setAllLEDs(pSharedPru0, 0);
            pSharedPru0->LEDS[LED_LEN - 1] = BRIGHT_GREEN;
            break;
        case RIGHT_DOWN:
            turnOnBottomLEDs(pSharedPru0, GREEN);
            break;
        case RIGHT_FAR_DOWN:
            setAllLEDs(pSharedPru0, 0);
            pSharedPru0->LEDS[0] = BRIGHT_GREEN;
            break;
        case RIGHT:
            setAllLEDs(pSharedPru0, GREEN);
            break;
        case CENTER_DOWN:
            turnOnBottomLEDs(pSharedPru0, BLUE);
            break;
        case CENTER_FAR_DOWN:
            setAllLEDs(pSharedPru0, 0);
            pSharedPru0->LEDS[0] = BRIGHT_BLUE;
            break;
        case CENTER_UP:
            turnOnTopLEDs(pSharedPru0, BLUE);
            break;
        case CENTER_FAR_UP:
            setAllLEDs(pSharedPru0, 0);
            pSharedPru0->LEDS[LED_LEN - 1] = BRIGHT_BLUE;
            break;
        case CENTER:
            setAllLEDs(pSharedPru0, BLUE);
            break;
        }
        sleepForMs(100);
    }
    return 0;
}

void freePruMmapAddr(volatile void *pPruBase)
{
    if (munmap((void *)pPruBase, PRU_LEN))
    {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}

void *JoystickThread(void *arg)
{
    volatile void *pPruBase = getPruMmapAddr();
    volatile sharedMemStruct_t *pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);
    int flag = 0;
    while (!stopped)
    {
        if (pSharedPru0->clickDown)
        {
            if(flag != 0) continue;
            flag = 1;
            if(fire()) {
                printf("\nHit\n");
            } else {
                printf("\nMISS\n");
            }
        }
        else if (pSharedPru0->clickRight)
        {
            stopped = 1;
        } else {
            flag = 0;
        }
        sleepForMs(1);
    }
    return 0;
}

void initializeJoystick()
{   
    runCommand("config-pin p8_15 pruin");
    runCommand("config-pin p8_16 pruin");
}

int main(void)
{
    // Get access to shared memory for my uses
    volatile void *pPruBase = getPruMmapAddr();
    volatile sharedMemStruct_t *pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);

    initializeLEDs();
    initializeJoystick();
    initThreads();
    while(!stopped) {sleepForMs(1);}
    stopThreads();

    // turn off all LEDs
    setAllLEDs(pSharedPru0, 0);

    // // Drive it
    // for (int i = 0; i < 20; i++) {
    //     // Drive LED
    //     pSharedPru0->isLedOn = (i % 2 == 0);

    //     // Print button
    //     printf("Button: %d\n",
    //         pSharedPru0->isButtonPressed);

    //     // Timing
    //     sleep(1);
    // }

    // Cleanup
    freePruMmapAddr(pPruBase);
}