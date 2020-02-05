#ifndef __INPUTS_H__
#define __INPUTS_H__

#define CONTROLLER_1 (unsigned int*)0xBFC007C4
#define CONTROLLER_2 (unsigned int*)0xBFC007CC

#define L_INPUT 0x15
#define CL_INPUT 0x11
#define CR_INPUT 0x10
#define CD_INPUT 0x12
#define CU_INPUT 0x13
#define A_INPUT 0xFF
#define B_INPUT 0x1E
#define START_INPUT 0x1C

#include "utility.h"

/**
 * Reads controller inputs
 * Returns:
 *  1 if input
 *  0 if no input
 */
BOOLEAN read_button(unsigned int, unsigned int *);

#endif
