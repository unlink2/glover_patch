#ifndef __MEMORY_H__
#define __MEMORY_H__

#define TIMER_HW (short*)0x801FA754
#define GAME_MODE (unsigned char*)0x801E7530 // byte 04 = gameplay 02 = level select
#define PAUSE_FLAG (unsigned char*)0x801EC7D3 // 01 = paused

#define LAST_INPUT_2 (unsigned int*)0x800002C8
#define FRAME_ADVANCE (unsigned char*)0x800002CC

#endif
