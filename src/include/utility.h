#ifndef __UTILITY_H__
#define __UTILITY_H__

typedef unsigned char BOOLEAN;
enum BOOL_VALUE {
        FALSE,
        TRUE
};


unsigned int strlen(char *);

void memcpy(unsigned char *, unsigned char *, unsigned int size);
void memset(unsigned char *, unsigned char, unsigned int);

/**
 * Assert verifies an expression
 * and hangs the process in an infinite loop
 * if the assertion fails
 * Disable by defining NDEBUG
 */
void assert(int);

#endif
