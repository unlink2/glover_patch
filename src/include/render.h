#ifndef __RENDER_H__
#define __RENDER_H__

#include "typedefs.h"
#include "memory.h"

#define SCREEN_W 320
#define SCREEN_H 240

#define CHAR_W 8
#define CHAR_H 8

void render();

HWORD_T *get_frame_buffer();

/**
 * Draws a string
 */
void gputs(char *, HWORD_T *, WORD_T, WORD_T, const WORD_T *, HWORD_T, HWORD_T);

/**
 * Draws a char to the framebuffer
 */
void draw_char(char, HWORD_T *, WORD_T, WORD_T, const WORD_T *, HWORD_T, HWORD_T);

/**
 * Write to framebuffer, the value is 2 bytes per pixel
 * RGBA format
 */
void write_to_framebuffer(HWORD_T *, HWORD_T, WORD_T);

#define __FB_WRITE_HW(pframebuffer, color, offset) pframebuffer[offset] = color;

#endif
