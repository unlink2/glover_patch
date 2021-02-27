#ifndef __SAVEFILE_H__
#define __SAVEFILE_H__

typedef struct save_file {
    char filename[4];
    u32 levels_cleared[2];
    u32 score;
    u8 unknown_1;
    u8 difficulty; // 00 = easy, 01 == normal
    u8 ball_counter;
    u8 hub_progress;
    u32 unknown_2;
    u32 settings; // sound, etc 
} save_file;

#endif
