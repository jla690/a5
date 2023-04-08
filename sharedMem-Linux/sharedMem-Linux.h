#ifndef _SHARED_MEM_LINUX_H_
#define _SHARED_MEM_LINUX_H_

#include "sharedDataStruct.h"
volatile void * getPruMmapAddr(void);

// turns on the bottom 3 LEDs
void turnOnBottomLEDs(volatile sharedMemStruct_t *pSharedPru0, int color);

// logic for determining which LED pattern to display
void *LEDThread(void *arg);

// sets all LEDs to the same color
void setAllLEDs(volatile sharedMemStruct_t *pSharedPru0, int color);

// turns on the top 3 LEDs
void turnOnTopLEDs(volatile sharedMemStruct_t *pSharedPru0, int color);

// initializes the LEDs
void initializeLEDs();

// frees the mmap address
void freePruMmapAddr(volatile void *pPruBase);

#endif