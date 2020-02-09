#ifndef __MEMWATCH_H__
#define __MEMWATCH_H__

#include "utility.h"
#include "memory.h"

#define WORDS_PER_PAGE 16
#define BYTES_PER_LINE 4

typedef struct memwatch {
    //  7th bit == 1 -> Watch enable flag
    HWORD_T flags; // this really only needs to be 1 byte, but it is word for padding
    HWORD_T cursor_pos; // cursor position, if FFFF it will select the address value
    WORD_T offset;
    BYTE_T *pstr; // string buffer, should have at least 18 bytes
    WORD_T base_addr; // start address of ram
} memwatch;


memwatch* memwatch_from_addr(WORD_T *);

void render_memwatch(memwatch *);

void update_memwatch(memwatch *);

#endif
