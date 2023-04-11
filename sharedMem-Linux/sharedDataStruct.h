// Header for shared memory structure
#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>

// My Shared Memory Structure
// ----------------------------------------------------------------
typedef struct {
    _Alignas(4*8) uint32_t LEDS[8];
    bool clickRight;
    bool clickDown;
    char _p0, _p1;
} sharedMemStruct_t;

#endif
