#ifndef __MEMWATCH_H__
#define __MEMWATCH_H__

#include "utility.h"
#include "memory.h"

#define WORDS_PER_PAGE 16
#define BYTES_PER_LINE 4
#define MEMWATCH_POLLING_RATE 4 // every N frames

typedef enum watch_type {
    NO_WATCH,
    BYTE_WATCH,
    HWORD_WATCH,
    WORD_WATCH
} watch_type;

typedef struct memwatch {
    //  7th bit == 1 -> Viewer enable flag
    //  6th bit == 1 -> Watch select flag (overrides viewer enable)
    BYTE_T flags; // this really only needs to be 1 byte, but it is word for padding
    BYTE_T frame_counter; // used for polling rate
    HWORD_T cursor_pos; // cursor position, if FFFF it will select the address value
    WORD_T offset;
    BYTE_T *pstr; // string buffer, should have at least 18 bytes
    WORD_T base_addr; // start address of ram
    WORD_T *watch_addr; // memory watch address if NULL ignore
    watch_type watch_type;
} memwatch;

extern memwatch pmemwatch;

void init_memwatch(memwatch *);

/**
 * Renders one specific address at all times
 * Polled every frame
 */
void render_watch_addr(memwatch *);

/**
 * Renders watch select menu
 */
void render_watchaddr(memwatch *);
void render_memwatch(memwatch *);

void prepware_watchaddr(memwatch *);

void prepare_watchselect(memwatch *);

/**
 * Prepares memwatch for rendering
 */
void prepare_memwatch(memwatch *);

void update_memwatch(memwatch *);

#define memwatch_current_addr(pmw) (BYTE_T*)pmw->base_addr+(pmw->offset*WORDS_PER_PAGE*sizeof(WORD_T))

#endif
