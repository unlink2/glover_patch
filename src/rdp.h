#ifndef __RDP_H__
#define __RDP_H__

#include "typedefs.h"
#include "utility.h"

#define RDP_WAIT_PIPE(other) while( (((volatile WORD_T*)0xA4100000)[3] & (0x600 | other)) ) ;

#define RDP_CLEAR_BUFFER(pbuffer, size) gmemset((BYTE_T*)pbuffer, 0x00, sizeof(WORD_T)*size);

// TODO rdp buffers are being rather wasteful

// rdp command buffer function

/**
 * Get global command buffer
 */
WORD_T *get_pbuffer();

/**
 * Increment global command buffer by
 * size bytes. Each command returns its size
 */
void inc_pbuffer(u32);
/**
 * Set global command buffer ptr
 */
void set_pbuffer(WORD_T *ptr);

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
int rdp_sync_tile(WORD_T *);
int rdp_sync_load(WORD_T *);

int rdp_draw_primitives(WORD_T *);
/**
 * Draws a rectangle from tx/ty to bx/by
 * the color is 2 16 bit colors in one word
 */
int rdp_draw_rect(WORD_T color, int, int, int, int, WORD_T *);

int rdp_texture_mode(WORD_T *);
int rdp_load_tile(HWORD_T *, WORD_T *);
int rdp_draw_tile(int, int, int, int, WORD_T *);


#endif 
