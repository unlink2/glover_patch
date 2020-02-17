#ifndef __RENDER_H__
#define __RENDER_H__

#include "typedefs.h"
#include "memory.h"

#define SCREEN_W 320
#define SCREEN_H 240

#define CHAR_W 8
#define CHAR_H 8

#define RDP_WAIT_PIPE(other) while( (((volatile WORD_T*)0xA4100000)[3] & (0x600 | other)) ) ;

void render();

// rdp test functions, do not use yet!

/*
 * send dl to rdp
 * This is an adaptation of the function in libdragon.
 * A free and open source n64 library in the public domain
 * https://github.com/DragonMinded/libdragon
 */
void rdp_send_dl(WORD_T *, WORD_T *);

/**
 * Performs a full sync and
 * sets the framebuffer
 * call before starting operations
 */
int rdp_sync_full(WORD_T *);
int rdp_sync_pipe(WORD_T *);

/**
 * Draws a rectangle from tx/ty to bx/by
 * the color is 2 16 bit colors in one word
 */
int rdp_draw_rect(WORD_T color, int, int, int, int, WORD_T *);

int rdp_texture_mode(WORD_T *);
int rdp_load_char(char, HWORD_T *, WORD_T *);

int rdp_draw_char(WORD_T *);

HWORD_T *get_frame_buffer();

/**
 * In game puts
 */
void puts_xy(char *str, unsigned int x, unsigned int y);

/**
 * Decompresses a font to a location in DRAM
 */
void decompress_font(WORD_T *, HWORD_T *, HWORD_T, HWORD_T);

/**
 * Draws a string
 */
void gputs(char *, HWORD_T *, WORD_T, WORD_T, const WORD_T *, HWORD_T, HWORD_T);

/**
 * Draws a char to the framebuffer
 * if the bg color is 0xNNN0 it will not be drawn because it is fully transparent
 */
void draw_char(char, HWORD_T *, WORD_T, WORD_T, const WORD_T *, HWORD_T, HWORD_T);


/**
 * Not really DMA yet, but maybe in the future
 * Requires a decompressed font instead of the data from rom
 */
void gputsf(char *, HWORD_T *, WORD_T, WORD_T, HWORD_T *);

/**
 * Not really DMA yet, but maybe in the future
 * Requires a decompressed font instead of the data from rom
 * should be faster than draw_char. requires less calculations.
 * X coordiniate must be evenly divisible by 4
 */
void draw_charf(char, HWORD_T *, WORD_T, WORD_T, HWORD_T *);

/**
 * Write to framebuffer, the value is 2 bytes per pixel
 * RGBA format
 */
void write_to_framebuffer(HWORD_T *, HWORD_T, WORD_T);

// guaranteed inline write to framebuffer
#define FB_WRITE_HW(pframebuffer, color, offset) pframebuffer[offset] = color;
#define FB_WRITE_W(pframebuffer, color, offset) *((WORD_T*)pframebuffer+offset) = color;

// guaranteed inline rendering of a character. if speed is not an issue call the
// function instead
#define INLINE_DRAW_CHAR(c, pframebuffer, x, y, pcharset, fg, bg) /* initial character offset */\
    const WORD_T *pchar = pcharset+(WORD_T)c*8;\
    HWORD_T offset = SCREEN_W * y + x; /* start address of framebuffer */\
    for (int y = 0; y < CHAR_H; y++) {\
        HWORD_T fdata = pchar[0];\
        for (int x = 0; x < CHAR_W; x++) {\
            HWORD_T set = fdata & 1 << x;\
            if (set) {\
                FB_WRITE_HW(pframebuffer, fg, offset);\
            } else if ((bg & 0x000F) != 0) {\
                FB_WRITE_HW(pframebuffer, bg, offset);\
            }\
            offset++; /* +1 offset */\
        }\
        pchar++; /* +1 font pixel */\
        offset += SCREEN_W-CHAR_W; /* next line */\
    }

// guaranteed inline rendering of a character. if speed is not an issue call the
// function instead
#define INLINE_DRAW_CHARF(c, pframebuffer, x, y, pfont) /* first pixel data*/\
    HWORD_T *pchar = pfont+(WORD_T)c*CHAR_W*CHAR_H;\
    HWORD_T offset = SCREEN_W/2 * y + x/2; /* start address of framebuffer */\
    for (int y = 0; y < CHAR_H; y++) {\
        for (int x = 0; x < CHAR_W/2; x++) {\
            FB_WRITE_W(pframebuffer, ((WORD_T*)pchar)[0], offset);\
            offset++; /* +1 offset */\
            pchar+=2; /* +1 font pixel */\
        }\
        offset += SCREEN_W/2-CHAR_W/2; /* next line */\
    }

#endif
