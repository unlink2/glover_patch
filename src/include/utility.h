#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "typedefs.h"

typedef unsigned char BOOLEAN;
enum BOOL_VALUE {
        FALSE,
        TRUE
};

#ifndef __LP64__
// create a pointer from an address
#define get_ptr(type, name, addr, size) type *name = (type*)addr;
#define free_ptr(ptr)
#else
#define UNIT_TESTS

#include <string.h>
#include <stdlib.h>

void* my_malloc(size_t);
void my_free(void *);

// malloc pointers gotten that way
#define get_ptr(type, name, addr, size) type *name = my_malloc(sizeof(type) * size);
#define free_ptr(ptr) my_free(ptr);
#endif

unsigned int gstrlen(char *);

void gmemcpy(BYTE_T *, BYTE_T *, unsigned int);
void gmemset(BYTE_T *, unsigned char, unsigned int);

/**
 * Assert verifies an expression
 * and hangs the process in an infinite loop
 * if the assertion fails
 * Disable by defining NDEBUG
 */
void gassert(int);

/**
 * Converts a word to string
 * The string buffer must have enough space to
 * fit a hex string for 4 bytes + \00
 */
void word_to_hexstr(WORD_T, char *);

#endif
