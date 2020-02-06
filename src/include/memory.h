#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "typedefs.h"

#define TIMER_HW (HWORD_T*)0x801FA754
#define GAME_MODE (BYTE_T*)0x801E7530 // byte 04 = gameplay 02 = level select
#define PAUSE_FLAG (BYTE_T*)0x801EC7D3 // 01 = paused

#define LAST_INPUT_2 (WORD_T*)0x800002C8
#define FRAME_ADVANCE (BYTE_T*)0x800002CC

// memwatch struct location
#define MEMWATCH_STRUCT (WORD_T*)0x800002D0

#define FILE1_START (WORD_T*)0x801EAA44

#define EXP_RAM (WORD_T*)0x80400000 // freely available ram!
#define X_BAC (WORD_T*)0x800002B0
#define Y_BAC (WORD_T*)0x800002B4
#define Z_BAC (WORD_T*)0x800002B8

#define GLOVER_X (WORD_T*)0x8029030C
#define GLOVER_Y (WORD_T*)0x80290310
#define GLOVER_Z (WORD_T*)0x80290314
#define GLOVER_VEL_X (WORD_T*)0x80290354
#define GLOVER_VEL_Y (WORD_T*)0x80290338
#define GLOVER_VEL_Z (WORD_T*)0x8029033C

#define BALL_X (WORD_T*)0x8029F9AC
#define BALL_Y (WORD_T*)0x8029F9B0
#define BALL_Z (WORD_T*)0x8029F9B4

#define ACTOR_HEAP_START (WORD_T*)0x802902D8
#define ACTOR_HEAP_CLONE (WORD_T*)0x80400000
#define ACTOR_SIZE 0xF0
#define RNG_VALUE (WORD_T*)0x801ED3F0

#define STRING_BUFFER (BYTE_T*)0x800002E0 // 32 bytes to store temporary strings for printing
#define PUTS_XY (WORD_T*)0x8014666C // puts function ptr
#define PUTS_X_POS (WORD_T*)0x801ED394
#define PUTS_Y_POS (WORD_T*)0x801ED398

#define CURRENT_MAP (BYTE_T*)0x801E7531

void init_mem();

#endif
