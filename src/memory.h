#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "typedefs.h"

#define TIMER_HW (HWORD_T*)0x801FA754
#define GAME_MODE (BYTE_T*)0x801E7530 // byte 04 = gameplay 02 = level select
#define GAME_STATE (BYTE_T*)0x801EC7F0 // 02 == demo
#define PAUSE_FLAG (BYTE_T*)0x801EC7D3 // 01 = paused
#define DISABLE_PAUSE_FLAG (BYTE_T*)0x801EC748 // 00 disables pause

#define IS_PAUSED (BYTE_T*)0x801EC7D2 // 01 if load was hit or game is paused
#define IGT (WORD_T*)0x80290198 // in game time
#define IN_GOAL1 (HWORD_T*)0x801e7466 // if != 0 glover will stick to goal
#define IN_GOAL2 (HWORD_T*)0x801E7620 // if != 0 glover will also stick to goal
#define WIN_LEVEL (HWORD_T*)0x801E7466 // if != 0 dec until 0 then win
#define FRAME_COUNTER (WORD_T*)0x802004E0 // frames since level loaded
#define IGT_WAIT_TARGET (HWORD_T*)0x801E76D4 // dont start igt until this value * 0x14 < FRAME_COUNTER

extern WORD_T *LAST_INPUT_1;
extern WORD_T *LAST_INPUT_2;
// #define LAST_INPUT_1 (WORD_T*)0x800002C4
// #define LAST_INPUT_2 (WORD_T*)0x800002C8
// #define FRAME_ADVANCE (BYTE_T*)0x800002CC

#define GAME_MODE (BYTE_T*)0x801E7530 // 01 == main menu; 03 == freeze; 04 == in-game

#define FILE1_START (WORD_T*)0x801EAA44

#define EXP_RAM (WORD_T*)0x80400000 // freely available ram!
#define EXP_RAM_END (BYTE_T*)0x807FFFFF // last byte of ram

#define GLOVER_ACTOR (WORD_T*)0x802902D8
#define BALL_ACTOR (WORD_T*)0x8029F978

#define GLOVER_X (WORD_T*)0x8029030C
#define GLOVER_Y (WORD_T*)0x80290310
#define GLOVER_Z (WORD_T*)0x80290314
#define GLOVER_VEL_X (WORD_T*)0x80290354
#define GLOVER_VEL_Y (WORD_T*)0x80290338
#define GLOVER_VEL_Z (WORD_T*)0x8029033C

#define GLOVER_HP (WORD_T*)0x80290194
#define GLOVER_LIVES (WORD_T*)0x80290190

#define INFINITE_DOUBLE_JUMP (BYTE_T*)0x8028FC07

#define BALL_X (WORD_T*)0x8029F9AC
#define BALL_Y (WORD_T*)0x8029F9B0
#define BALL_Z (WORD_T*)0x8029F9B4

#define ACTOR_HEAP_START (WORD_T*)0x802902D8
#define ACTOR_HEAP_CLONE (WORD_T*)0x80600000
#define ACTOR_HEAP_SIZE 0x20000/4 // padding at end of heap clone

#define ACTOR_SIZE 0xF0
#define RNG_VALUE (WORD_T*)0x801ED3F0
#define RNG_FUNC (void*)0x80147000

#define DEMO_END_TIMER (BYTE_T*)0x801EC775

#define PUTS_XY (WORD_T*)0x8014666C // puts function ptr
#define PUTS_X_POS (WORD_T*)0x801ED394
#define PUTS_Y_POS (WORD_T*)0x801ED398

#define COLLISION_DISABLE (WORD_T*)0x8015D728
#define FOG (BYTE_T*)0x801ED403

#define DISABLE_INPUT_TIMER (HWORD_T*)0x8028FBF4

// offset is 4 seconds by default.
#define TIMER_IGT_OFFSET_DEFAULT 4;
#define TIMER_IGT_OFFSET (HWORD_T*)0x801E76D4

#define __OS_DISABLE_INT (WORD_T*)0x801C6F70 // function ptr void ()
#define __OS_RESTORE_INT (WORD_T*)0x801C6F90 // function ptr void ()
#define __OS_GET_CURRENT_FRAMEBUFFER (WORD_T*)0x801CE910 // function ptr WORDL_T *()


// amount of available ram
#define RD_RAM_SIZE (WORD_T*)0x80000318

#define CURRENT_MAP (BYTE_T*)0x801E7531

// both need to be modifed at once
#define FRAME_RATE_1 (HWORD_T*)0x801F4702
#define FRAME_RATE_2 (HWORD_T*)0x801F4732

#define DISABLE_INT 0x801C6F70
#define RESTORE_INT 0x801C6F90
#define OS_GET_COUNT 0x801C62B0 // counter that increments at 1/2 cpu speed
#define FRAME_RENDERED (WORD_T*)0x80200980 // is set to 0x40 when frame is actually ready

// variables


#define RDP_DL_BUFFER_PTR 0x80529D3C // points to dl pbuffer
// 0x500 bytes for display list stuff
// each DL Buffer contains an upper and lower half for double buffering
#define RDP_DL_SIZE 0x30000
#define RDP_DL_BUFFER 0x80529D40
#define RDP_DL_END 0x805e9d40
#define RDP_BUFFERS 4

// uncompressed location of font 0x4000 bytes
// TODO make sure this is a good location
#define FONT8X8 (WORD_T*)0x80525C30
#define FONT8X8_HI (WORD_T*)0x80521c30 // highlight font location

// 0x20 strings of 0x10 bytes each. will be rendered to the screen
// every frame if needed
#define SCREEN_BUFFER (BYTE_T*)0x80500000


#define X_BAC (WORD_T*)0x805002B0
#define Y_BAC (WORD_T*)0x805002B4
#define Z_BAC (WORD_T*)0x805002B8

#define ENABLE_GRAVITY (BYTE_T*)0x801EEB60

// TODO find better size
#define CAMERA_ACTOR_SIZE ACTOR_SIZE*2
#define CAMERA_ACTOR (WORD_T*)0x8028F8E0
#define CAMERA_DATA (WORD_T*)0x80202248

#define CAMERA_MATRIX (WORD_T *)0x80206300

#define SWITCH_LIST_START (WORD_T *)0x802994b0
#define GARIB_LIST_START (WORD_T *)0x80332690 // TODO this is not correct!

#define TRIGGER_CHEAT (void (*)(int, int, int))0x801BB9AC // function(cheat_number, 1, 8) ??
#define LOAD_MAP (void (*)(int))0x8011FF40 // function(map_id)
#define FADE (void (*))0x8014D4D0 // function()
#define INIT_LOAD (void (*)(int))0x8011CD50 // function(1) ??

#define OBJ_BANK (WORD_T*)0x8025D698

#define LEVEL_SELECT (BYTE_T*)0x8011E653

#define FADE_TIMER (HWORD_T*)0x801E753C
#define LOAD_FADE (HWORD_T*)0x801E753C // is loading finished?
#define TARGET_LOAD_FADE (HWORD_T*)0x801E753E

#define DEBUG_GRAPH (BYTE_T*)0x801EFCA0 // nonzero enables graph

#define PAUSE_MENU_ANIMATION (BYTE_T*)0x801E765F

/**
 * This is a volotile ptr. follow obj bank  ObjBank.mp or TexBank
 */
// #define MODEL_DATA_TABLE (void*)0x80329D50 // list of all loaded models. zero terminated

#define BALL_STORAGE (WORD_T*)0x801EFD00 // is set to 0x2A if ball enters a loading zone

void init_mem();

#endif
