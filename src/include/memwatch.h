#ifndef __MEMWATCH_H__
#define __MEMWATCH_H__

#include "utility.h"
#include "memory.h"

typedef struct memwatch {
    //  7th bit == 1 -> Watch enable flag
    WORD_T flags; // this really only needs to be 1 byte, but it is word for padding
    WORD_T offset;
    BYTE_T *pstr; // string buffer, should have at least 18 bytes
    WORD_T base_addr; // start address of ram
} memwatch;


memwatch* memwatch_from_addr(WORD_T *);

void render_memwatch(memwatch *);

void update_memwatch(memwatch *);

#endif
