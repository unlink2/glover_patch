#ifndef __GPUTILITY_H__
#define __GPUTILITY_H__

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
int gstrncmp(char *, char *, unsigned int);

void gstrcpy(char *, char *);

void *gmemcpy(void *src, void *dest, unsigned int n);
void *_gmemcpy(void *dest, void *src, unsigned int n);
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
void to_hexstr(WORD_T, char *, WORD_T);
void to_hexstr_signed(WORD_T, char *, WORD_T);

void str_reverse(char *, int);

int to_decstr(WORD_T, char *, WORD_T);
void to_floatstr(float, char *, WORD_T);
void to_floatstr_pad(float, char *, WORD_T, BYTE_T);

int gpow(int, int);

WORD_T from_hexstr(char *, int);

void split_space(char *, char **, char **);

// simple command parser

typedef struct arg {
    const char *key;
    const char *value;
} arg;

arg parse_arg(char *, const char *);

BOOLEAN is_arg(char *, const char *);

#endif
