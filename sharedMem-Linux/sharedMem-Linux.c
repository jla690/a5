#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>

#include "sharedDataStruct.h"
#include "accelerometer.h"
#include "utils.h"
#include "thread_control.h"

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
#define PRU_ADDR      0x4A300000   // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN       0x80000      // Length of PRU memory
#define PRU0_DRAM     0x00000      // Offset to DRAM
#define PRU1_DRAM     0x02000
#define PRU_SHAREDMEM 0x10000      // Offset to shared memory
#define PRU_MEM_RESERVED 0x200     // Amount used by stack and heap

#define GREEN 0x0f000000
#define RED 0x000f0000
#define BLUE 0x00000f00

// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ( (base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ( (base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ( (base) + PRU_SHAREDMEM)

// Return the address of the PRU's base memory
volatile void* getPruMmapAddr(void)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }

    // Points to start of PRU memory.
    volatile void* pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pPruBase;
}

void turnOnBottmLEDs(volatile sharedMemStruct_t* pSharedPru0, int color) {
    for (int i = 0; i < 3; i++) {
        pSharedPru0->LEDS[i] = color;
    }
    for (int i = 3; i < 8; i++) {
        pSharedPru0->LEDS[i] = 0;
    }
}

void turnOnAllLEDs(volatile sharedMemStruct_t* pSharedPru0, int color) {
    for (int i = 0; i < 8; i++) {
        pSharedPru0->LEDS[i] = color;
    }
}

void turnOnTopLEDs(volatile sharedMemStruct_t* pSharedPru0, int color) {
    for (int i = 0; i < 4; i++) {
        pSharedPru0->LEDS[i] = 0;
    }
    for (int i = 4; i < 8; i++) {
        pSharedPru0->LEDS[i] = color;
    }
}

void freePruMmapAddr(volatile void* pPruBase)
{
    if (munmap((void*) pPruBase, PRU_LEN)) {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    // Get access to shared memory for my uses
    volatile void *pPruBase = getPruMmapAddr();
    volatile sharedMemStruct_t *pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);

    initThreads();
    turnOnAllLEDs(pSharedPru0, GREEN);
    sleep(60);
    stopThreads();

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