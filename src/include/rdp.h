#ifndef __RDP_H__
#define __RDP_H__

#include "typedefs.h"

#define RDP_WAIT_PIPE(other) while( (((volatile WORD_T*)0xA4100000)[3] & (0x600 | other)) ) ;

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
int rdp_sync_load(WORD_T *);

/**
 * Draws a rectangle from tx/ty to bx/by
 * the color is 2 16 bit colors in one word
 */
int rdp_draw_rect(WORD_T color, int, int, int, int, WORD_T *);

int rdp_texture_mode(WORD_T *);
int rdp_load_char(char, HWORD_T *, WORD_T *);

int rdp_draw_char(WORD_T *);



#endif 
