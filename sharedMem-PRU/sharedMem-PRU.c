#include "../sharedMem-Linux/sharedDataStruct.h"
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <stdbool.h>
#include <stdint.h>

// GPIO Configuration
// ----------------------------------------------------------
volatile register uint32_t __R30; // output GPIO register
volatile register uint32_t __R31; // input GPIO register

// GPIO Output: P8_12 = pru0_pru_r30_14
//   = LEDDP2 (Turn on/off right 14-seg digit) on Zen cape
#define DIGIT_ON_OFF_MASK (1 << 14)
// GPIO Input: P8_15 = pru0_pru_r31_15
//   = JSRT (Joystick Right) on Zen Cape
//   (Happens to be bit 15 and p8_15; just a coincidence; see P8 header info
//   sheet)
#define JOYSTICK_RIGHT_MASK (1 << 15)
#define JOYSTICK_DOWN_MASK (1 << 14)

#define STR_LEN 8           // # LEDs in our string
#define oneCyclesOn 700 / 5 // Stay on 700ns
#define oneCyclesOff 800 / 5
#define zeroCyclesOn 350 / 5
#define zeroCyclesOff 600 / 5
#define resetCycles 60000 / 5 // Must be at least 50u, use 60u

#define DATA_PIN 15

// Shared Memory Configuration
// -----------------------------------------------------------
#define THIS_PRU_DRAM 0x00000 // Address of DRAM
#define OFFSET 0x200 // Skip 0x100 for Stack, 0x100 for Heap (from makefile)
#define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)

// This works for both PRU0 and PRU1 as both map their own memory to 0x0000000
volatile sharedMemStruct_t *pSharedMemStruct =
    (volatile void *)THIS_PRU_DRAM_USABLE;

void main(void)
{
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    while (1) {
        __delay_cycles(resetCycles);

        // Sets lights from shared struct
        for (int j = 0; j < STR_LEN; j++) {
            for (int i = 31; i >= 0; i--) {
                if (pSharedMemStruct->LEDS[j] & ((uint32_t)0x1 << i)) {
                    __R30 |= 0x1 << DATA_PIN; // Set the GPIO pin to 1
                    __delay_cycles(oneCyclesOn - 1);
                    __R30 &= ~(0x1 << DATA_PIN); // Clear the GPIO pin
                    __delay_cycles(oneCyclesOff - 2);
                }
                else {
                    __R30 |= 0x1 << DATA_PIN; // Set the GPIO pin to 1
                    __delay_cycles(zeroCyclesOn - 1);
                    __R30 &= ~(0x1 << DATA_PIN); // Clear the GPIO pin
                    __delay_cycles(zeroCyclesOff - 2);
                }
            }
        }

        pSharedMemStruct->clickRight = (__R31 & JOYSTICK_RIGHT_MASK) == 0;
        pSharedMemStruct->clickDown = (__R31 & JOYSTICK_DOWN_MASK) == 0;

        // Send Reset
        __R30 &= ~(0x1 << DATA_PIN); // Clear the GPIO pin
        __delay_cycles(resetCycles);
    }

    __halt();
}
