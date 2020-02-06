#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "typedefs.h"

typedef unsigned char BOOLEAN;
enum BOOL_VALUE {
        FALSE,
        TRUE
};


unsigned int strlen(char *);

void memcpy(BYTE_T *, BYTE_T *, unsigned int);
void memset(BYTE_T *, unsigned char, unsigned int);

/**
 * Assert verifies an expression
 * and hangs the process in an infinite loop
 * if the assertion fails
 * Disable by defining NDEBUG
 */
void assert(int);

/**
 * Converts a word to string
 * The string buffer must have enough space to
 * fit a hex string for 4 bytes + \00
 */
void word_to_hexstr(WORD_T, char *);

#endif
