#ifndef __MEMWATCH_H__
#define __MEMWATCH_H__

#include "utility.h"
#include "memory.h"

#define WORDS_PER_PAGE 16
#define BYTES_PER_LINE 4
#define MEMWATCH_POLLING_RATE 4 // every N frames

typedef struct memwatch {
    //  7th bit == 1 -> Watch enable flag
    BYTE_T flags; // this really only needs to be 1 byte, but it is word for padding
    BYTE_T frame_counter; // used for polling rate
    HWORD_T cursor_pos; // cursor position, if FFFF it will select the address value
    WORD_T offset;
    BYTE_T *pstr; // string buffer, should have at least 18 bytes
    WORD_T base_addr; // start address of ram
} memwatch;


memwatch* memwatch_from_addr(WORD_T *);

void render_memwatch(memwatch *);

/**
 * Prepares memwatch for rendering
 */
void prepare_memwatch(memwatch *);

void update_memwatch(memwatch *);

#endif
