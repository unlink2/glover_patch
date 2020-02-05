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

    // controller 2

    if (read_button(CU_INPUT, CONTROLLER_2)
        && !read_button(CU_INPUT, LAST_INPUT_2)) {
        unsigned char *pframe_advance = FRAME_ADVANCE;
        *pframe_advance = 2; // set to 2 to prevent double input

        // store last input values
        store_inputs(CONTROLLER_2, LAST_INPUT_2);
    }

    frame_advance();

    // store last input values
    store_inputs(CONTROLLER_2, LAST_INPUT_2);
}

void enable_timer() {
    short *ptr = TIMER_HW;
    (*ptr) = 0xFF;
}

void level_select() {
    unsigned char *pgamemode_ptr = GAME_MODE;
    unsigned char *ppause_ptr = PAUSE_FLAG;
    *pgamemode_ptr = 0x02; // enable level select
    *ppause_ptr = 0x00; // disable pause
}

void frame_advance() {
    unsigned char *pframe_advance = FRAME_ADVANCE;
    unsigned int *plast_z_write = (unsigned int*)0x801349B4;
    while (*pframe_advance) {
        if (read_button(CD_INPUT, CONTROLLER_2)
                && *pframe_advance == 1) {
            *pframe_advance = 2;
            // restore instruction
            *plast_z_write = 0xA0440000; // original instruction
            break;
        } else if (!read_button(CD_INPUT, CONTROLLER_2)
                && !read_button(CU_INPUT, CONTROLLER_2)) {
            *pframe_advance = 1;
        } else if(read_button(CU_INPUT, CONTROLLER_2)
                && *pframe_advance == 1) {
            *pframe_advance = 0;
        }
        store_inputs(CONTROLLER_2, LAST_INPUT_2);

        // patch out bad code we dont want
        // this code prevents Z inputs from working correctly
        // replace with nop
        *plast_z_write = 0x00; // nop
    }
}
