/**
 * Outputs basic player info to the screen
 */

#ifndef __PLAYER_INFO_H__
#define __PLAYER_INFO_H__

#include "rdp.h"
#include "render.h"
#include "memwatch.h"

typedef struct playerinfo {
    // 7th bit == 1 -> enable
    u8 flags;
} playerinfo;

void update_playerinfo(playerinfo *, memwatch *);

#endif
