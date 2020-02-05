#include "include/logic.h"
#include "include/inputs.h"

void logic() {
    // only trigger this code if start is held
    if (read_button(START_INPUT, CONTROLLER_1)) {
        if (read_button(A_INPUT, CONTROLLER_1)) {
            enable_timer();
        }
        if (read_button(B_INPUT, CONTROLLER_1)) {
            level_select();
        }

    }
}

void enable_timer() {
    short *ptr = TIMER_HW;
    (*ptr) = 0xFF;
}

void level_select() {
    unsigned char *gamemode_ptr = GAME_MODE;
    unsigned char *pause_ptr = PAUSE_FLAG;
    *gamemode_ptr = 0x02; // enable level select
    *pause_ptr = 0x00; // disable pause
}
