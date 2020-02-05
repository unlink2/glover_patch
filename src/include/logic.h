#ifndef __LOGIC_H__
#define __LOGIC_H__

#define TIMER_HW (short*)0x801FA754
#define GAME_MODE (unsigned char*)0x801E7530 // byte 04 = gameplay 02 = level select
#define PAUSE_FLAG (unsigned char*)0x801EC7D3 // 01 = paused

void logic();

void enable_timer();
void level_select();

#endif 
