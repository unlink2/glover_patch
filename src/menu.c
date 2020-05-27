#include "include/render.h"
#include "include/menu.h"
#include "include/utility.h"
#include "include/font8x8_basic.h"
#include "include/inputs.h"
#include "include/logic.h"
#include "include/debug.h"
#include "include/utility.h"
#include "include/actor.h"


#define CHEATS_LEN 26
char *cheat_names[CHEATS_LEN];

menudef pmenu;

void init_default_menu(menudef *pmenu) {
    get_ptr(char, string_buffer, SCREEN_BUFFER, 0x20*0x10);
    pmenu->pstr = string_buffer;
    pmenu->size = 15;
    pmenu->cursor = 0;
    pmenu->strings[0] = "Memory Monitor";
    pmenu->strings[1] = "Memory Monitor ASCII";
    pmenu->strings[2] = "Save Position";
    pmenu->strings[3] = "Load Position";
    pmenu->strings[4] = "Save Actors Slot           ";
    pmenu->strings[5] = "Load Actors Slot           ";
    pmenu->strings[6] = "Start Timer";
    pmenu->strings[7] = "Level Select";
    pmenu->strings[8] = "Toggle Collision";
    pmenu->strings[9] = "Fog";
    pmenu->strings[10] = "Other...";
    pmenu->strings[11] = "FPS:            ";
    pmenu->strings[12] = "Init ED...";
    pmenu->strings[13] = "Clear watch...";
    pmenu->strings[14] = "Move Object...";

    pmenu->type[0] = MENU_BUTTON;
    pmenu->type[1] = MENU_BUTTON;
    pmenu->type[2] = MENU_BUTTON;
    pmenu->type[3] = MENU_BUTTON;
    pmenu->type[4] = MENU_VALUE_HWORD;
    pmenu->type[5] = MENU_VALUE_HWORD;
    pmenu->type[6] = MENU_BUTTON;
    pmenu->type[7] = MENU_BUTTON;
    pmenu->type[8] = MENU_BUTTON;
    pmenu->type[9] = MENU_BUTTON;
    pmenu->type[10] = MENU_BUTTON;
    pmenu->type[11] = MENU_VALUE_HWORD;
    pmenu->type[12] = MENU_BUTTON;
    pmenu->type[13] = MENU_BUTTON;
    pmenu->type[14] = MENU_BUTTON;

    pmenu->pvalue[4] = &pmenu->pgpatch->restore_slot;
    pmenu->pvalue[5] = &pmenu->pgpatch->restore_slot;
    pmenu->pvalue[11] = (void*)FRAME_RATE_1;

    pmenu->pactions = &main_menu_select;
    pmenu->pupdate = &main_menu_update;
}

u8 cheat_num = 0;
u8 map_id = 0;

void init_glover_menu(menudef *pmenu) {
    pmenu->size = 10;
    pmenu->cursor = 0;
    pmenu->strings[0] = "Toggle Infinite Lives";
    pmenu->strings[1] = "Toggle Infinite Health";
    pmenu->strings[2] = "Trigger Afterlife";
    pmenu->strings[3] = "Lock Position";
    pmenu->strings[4] = "Summon Ball";
    pmenu->strings[5] = "Toggle Infinite Jump";
    pmenu->strings[6] = "Toggle Space CS Skip";
    pmenu->strings[7] = "Cheat                  ";
    pmenu->strings[8] = "Lock RNG (may crash)";
    pmenu->strings[9] = "Load Map         ";

    pmenu->type[0] = MENU_BUTTON;
    pmenu->type[1] = MENU_BUTTON;
    pmenu->type[2] = MENU_BUTTON;
    pmenu->type[3] = MENU_BUTTON;
    pmenu->type[4] = MENU_BUTTON;
    pmenu->type[5] = MENU_BUTTON;
    pmenu->type[6] = MENU_BUTTON;
    pmenu->type[7] = MENU_VALUE_BYTE;
    pmenu->type[8] = MENU_BUTTON;
    pmenu->type[9] = MENU_VALUE_BYTE;

    pmenu->pvalue[7] = &cheat_num;
    pmenu->pvalue[9] = &map_id;

    pmenu->pactions = &glover_menu_select;
    pmenu->pupdate = &glover_menu_update;

    // init cheat names
    cheat_names[0] = "Locate Garibs";
    cheat_names[1] = "Call Ball";
    cheat_names[2] = "Checkpoints";
    cheat_names[3] = "Infinite Lives";
    cheat_names[4] = "Powerball";
    cheat_names[5] = "05";
    cheat_names[6] = "Infinite Energy";
    cheat_names[7] = "Enemy Ball";
    cheat_names[8] = "Low Gravity";
    cheat_names[9] = "Big Ball";
    cheat_names[10] = "Fish Eye";
    cheat_names[11] = "Rotate R";
    cheat_names[12] = "Rotate L";
    cheat_names[13] = "Mad";
    cheat_names[14] = "Death";
    cheat_names[15] = "Frog";
    cheat_names[16] = "Hercules";
    cheat_names[17] = "Speedup";
    cheat_names[18] = "12";
    cheat_names[19] = "13";
    cheat_names[20] = "Froggy";
    cheat_names[21] = "Secret";
    cheat_names[22] = "All Cheats";
    cheat_names[23] = "Level Select";
    cheat_names[24] = "Open Portals";
    cheat_names[25] = "Open Levels";

}

BYTE_T move_value;

void init_move_menu(menudef *pmenu) {
    pmenu->size = 14;
    pmenu->cursor = 0;
    pmenu->strings[0] = "Next Object";
    pmenu->strings[1] = "Prev Object";
    pmenu->strings[2] = "X+";
    pmenu->strings[3] = "X-";
    pmenu->strings[4] = "Y+";
    pmenu->strings[5] = "Y-";
    pmenu->strings[6] = "Z+";
    pmenu->strings[7] = "Z-";
    pmenu->strings[8] = "FFFFFFFF"; // is button but use as label only
    pmenu->strings[9] = "+/- Delta    "; // increase delta to move by
    pmenu->strings[10] = "Show/Hide";
    pmenu->strings[11] = "Save Object Bank";
    pmenu->strings[12] = "Load Object Bank";
    pmenu->strings[13] = "Show/Hide all objects";

    pmenu->type[0] = MENU_BUTTON;
    pmenu->type[1] = MENU_BUTTON;
    pmenu->type[2] = MENU_BUTTON;
    pmenu->type[3] = MENU_BUTTON;
    pmenu->type[4] = MENU_BUTTON;
    pmenu->type[5] = MENU_BUTTON;
    pmenu->type[6] = MENU_BUTTON;
    pmenu->type[7] = MENU_BUTTON;
    pmenu->type[8] = MENU_BUTTON;
    pmenu->type[9] = MENU_VALUE_BYTE;
    pmenu->type[10] = MENU_BUTTON;
    pmenu->type[11] = MENU_BUTTON;
    pmenu->type[12] = MENU_BUTTON;
    pmenu->type[13] = MENU_BUTTON;

    // set pvalue 0 to glover pointer
    pmenu->pvalue[0] = GLOVER_ACTOR;
    pmenu->pvalue[9] = &move_value;
    move_value = 1;

    pmenu->pactions = &move_object_select;
    pmenu->pupdate = &move_object_update;
}

void main_menu_select(menudef *pmenu) {
    switch(pmenu->cursor) {
        case 0:
            pmenu->pmemwatch->flags = pmenu->pmemwatch->flags ^ 0x80;
            // not ascii mode
            pmenu->pmemwatch->flags = pmenu->pmemwatch->flags & 0b11011111;
            pmenu->flags = 0x00;
            break;
        case 1:
            // ascii mode
            pmenu->pmemwatch->flags = pmenu->pmemwatch->flags ^ 0b10100000;
            pmenu->flags = 0x00;
            break;
        case 2:
            store_glover_pos();
            break;
        case 3:
            restore_glover_pos();
            break;
        case 4:
            clone_actors(NULL, pmenu->pgpatch->restore_slot);
            break;
        case 5:
            restore_actors(NULL, pmenu->pgpatch->restore_slot);
            break;
        case 6:
            enable_timer();
            break;
        case 7:
            level_select();
            break;
        case 8:
            toggle_collision();
            break;
        case 9:
            toggle_fog();
            break;
        case 10:
            init_glover_menu(pmenu);
            break;
        case 12:
            evd_init();
            break;
        case 13:
            clear_all_watch(pmenu->pmemwatch);
            break;
        case 14:
            init_move_menu(pmenu);
            break;
        default:
            pmenu->flags = 0x00;
            pmenu->cursor = 0;
            break;
    }
}


void main_menu_update(menudef *pmenu) {
    // put framerate number into framerate string
    HWORD_T *ptr = (HWORD_T*)pmenu->pvalue[11];
    char *str = pmenu->strings[11];
    to_hexstr(*ptr, str+5, 2);

    pmenu->pgpatch->restore_slot = pmenu->pgpatch->restore_slot & MAX_RESTORE_SLOTS;
    // restore to hex
    to_hexstr((WORD_T)pmenu->pgpatch->restore_slot, pmenu->strings[5]+gstrlen("Load Actors Slot "), 2);
    to_hexstr((WORD_T)pmenu->pgpatch->restore_slot, pmenu->strings[4]+gstrlen("Save Actors Slot "), 2);
}

void trigger_al(menudef *pmenu) {
    get_ptr(WORD_T, al_inst, 0x801256FC, 1);
    if (*al_inst == 0x1040001D) {
        *al_inst = 0x1000001D; // branch always

        // this byte determines which mad to load
        // based on the current world (801E7533)
        // 0F == mainmenu
        // 13 == boss of world
        // etc
        get_ptr(BYTE_T, al_value, 0x801E7541, 1);
        *al_value = 0x13;
        get_ptr(BYTE_T, game_over, 0x801E7473, 1);
        *game_over = 1;
        pmenu->strings[2] = "Restore Game Over Code";
        get_ptr(WORD_T, lives, 0x80290190, 1);
        *lives = 0x00-1;
    } else {
        pmenu->strings[2] = "Trigger Afterlife";
        *al_inst = 0x1040001D; // regulat instruction
    }
}

void glover_menu_select(menudef *pmenu) {
    switch(pmenu->cursor) {
        case 0:
            pmenu->pgpatch->infinite_hp = !pmenu->pgpatch->infinite_hp;
            break;
        case 1:
            pmenu->pgpatch->infinite_lives = !pmenu->pgpatch->infinite_lives;
            break;
        case 2:
            trigger_al(pmenu);
            break;
        case 3:
            pmenu->pgpatch->lock_pos = !pmenu->pgpatch->lock_pos;
            store_glover_pos();
            break;
        case 4:
            store_glover_pos();
            restore_glover_pos();
            break;
        case 5:
            pmenu->pgpatch->infinite_jump = !pmenu->pgpatch->infinite_jump;
            break;
        case 6:
            pmenu->pgpatch->cutscene_skip = !pmenu->pgpatch->cutscene_skip;
            break;
        case 7: {
            void (*trigger_cheat)(int, int, int) = TRIGGER_CHEAT;
            trigger_cheat((int)(*(u8*)(pmenu->pvalue[7])), 1, 8); // TODO what do other inputs do?
            break; }
        case 8: {
            pmenu->pgpatch->lockrng = !pmenu->pgpatch->lockrng;

            // start of rng function
            get_ptr(u32, rngval, RNG_FUNC, 1);
            if (pmenu->pgpatch->lockrng) {
                pmenu->strings[8] = "Unlock RNG";
                rngval[0] = 0x03E00008; // jr ra nop
                rngval[1] = 0x00;
            } else {
                pmenu->strings[8] = "Lock RNG";
                rngval[0] = 0x14800003; // original code
                rngval[1] = 0x27BDFFF8;
            }
            break; }
        case 9: {
            void (*load_map)(int) = LOAD_MAP;
            // void (*fade)() = FADE;
            void (*init_load)(int) = INIT_LOAD;
            //fade();
            init_load(1);
            load_map((int)(*(u8*)(pmenu->pvalue[9])));
            break; }
        default:
            init_default_menu(pmenu);
            break;
    }
}

void glover_menu_update(menudef *pmenu) {
    if (pmenu->pgpatch->infinite_hp) {
        pmenu->strings[0] = "Disable Infinite Health";
    } else {
        pmenu->strings[0] = "Enable Infinite Health";
    }
    if (pmenu->pgpatch->infinite_lives) {
        pmenu->strings[1] = "Disable Infinite Lives";
    } else {
        pmenu->strings[1] = "Enable Infinite Lives";
    }
    // cheat select menu

    BYTE_T *cheatnum = (BYTE_T*)pmenu->pvalue[7];
    // if it has a name add the name otherwise just add the number
    if (*cheatnum < CHEATS_LEN) {
        // to_hexstr((WORD_T)cheat_names, pmenu->strings[7]+gstrlen("Cheat "), 4);
        gstrcpy(pmenu->strings[7]+gstrlen("Cheat "), cheat_names[*cheatnum]);
    } else {
        to_hexstr((WORD_T)*cheatnum, pmenu->strings[7]+gstrlen("Cheat "), 1);
    }

    // map id to string
    to_hexstr((WORD_T)map_id, pmenu->strings[9]+gstrlen("Load Map "), 1);
}

void move_object_select(menudef *pmenu) {
    // get actor
    glover_actor *pactor = (glover_actor*)pmenu->pvalue[0];
    BYTE_T *delta = (BYTE_T*)pmenu->pvalue[9];
    get_ptr(glover_actor, pglover, GLOVER_ACTOR, 1);
    get_ptr(camera_t, pcamera, CAMERA_ACTOR, 1);

    switch (pmenu->cursor) {
        case 0:
            pmenu->pvalue[0] = pactor->pnext;
            break;
        case 1:
            pmenu->pvalue[0] = pactor->pprev;
            break;
        case 2:
            pactor->xpos += *delta;
            break;
        case 3:
            pactor->xpos -= *delta;
            break;
        case 4:
            pactor->ypos += *delta;
            break;
        case 5:
            pactor->ypos -= *delta;
            break;
        case 6:
            pactor->zpos += *delta;
            break;
        case 7:
            pactor->zpos -= *delta;
            break;
        case 8:
            // move glover to object
            pglover->xpos = pactor->xpos;
            pglover->ypos = pactor->ypos - 10.0f; // so we dont clip
            pglover->zpos = pactor->zpos;
            pcamera->xpos = pactor->xpos;
            pcamera->ypos = pactor->ypos;
            pcamera->zpos = pactor->zpos;
            break;
        case 10: {
            // toggle rendering
            if (pactor->visible_flag) {
                pactor->visible_flag = 0;
            } else {
                pactor->visible_flag = 64;
            }
            break; }
        case 11:
            clone_obj_bank();
            break;
        case 12:
            restore_actors(NULL, MAX_RESTORE_SLOTS+1);
            break;
        case 13:
            toggle_show_objects();
            break;
        default:
            init_default_menu(pmenu);
            break;
    }
}

void move_object_update(menudef *pmenu) {
    // update label
    to_hexstr((WORD_T)pmenu->pvalue[0], pmenu->strings[8], 4);
    BYTE_T *delta = (BYTE_T*)pmenu->pvalue[9];
    to_hexstr((WORD_T)*delta, pmenu->strings[9]+gstrlen("+/- Delta "), 1);
    // get current actor
    glover_actor *pactor = (glover_actor*)pmenu->pvalue[0];
    if (pactor->visible_flag) {
        pmenu->strings[10] = "Hide Actor";
    } else {
        pmenu->strings[10] = "Show Actor";
    }

    // easy show/hide
    if (read_button(R_INPUT, CONTROLLER_2)
            && !read_button(R_INPUT, LAST_INPUT_2)) {
        u32 select = pmenu->cursor; // store old cursor
        pmenu->cursor = 10; // toggle rendering
        move_object_select(pmenu);
        pmenu->cursor = select; // restore
    }
}

void render_menu(menudef *pmenu) {
    if ((pmenu->flags & 0x80) == 0) {
        return;
    }

    HWORD_T *pframebuffer = get_frame_buffer();
    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);
    // render menu if flag is enabled
    unsigned short start_x = 0x10;
    unsigned short start_y = 0x20;
    // display 16 bytes on screen 1 word per line
    for (int i = 0; i < pmenu->size; i++, start_y += CHAR_H+1) {
        gputsrdp(pmenu->strings[i], start_x, start_y, pfont);
    }


    unsigned int x_offset = start_x;
    unsigned int y_offset = (pmenu->cursor)*(CHAR_H+1)+0x20;

    // render cursor
    draw_char('_', pframebuffer, x_offset, y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
    draw_char('_', pframebuffer, x_offset+8, y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);

}

void update_menu(menudef *pmenu) {
    if (read_button(START_INPUT, CONTROLLER_2)
            && !read_button(START_INPUT, LAST_INPUT_2)) {
        pmenu->flags = pmenu->flags ^ 0x80;
        pmenu->pmemwatch->flags = 0x00;
        pmenu->pkb->flags = 0x00;
    }

    if ((pmenu->flags & 0x80) == 0) {
        return;
    }

    pmenu->pupdate(pmenu);

    if (read_button(A_INPUT, CONTROLLER_2)
            && !read_button(A_INPUT, LAST_INPUT_2)) {
        pmenu->pactions(pmenu);
    } else if (read_button(B_INPUT, CONTROLLER_2)
            && !read_button(B_INPUT, LAST_INPUT_2)) {
        pmenu->cursor = BACK_ACTION;
        pmenu->pactions(pmenu);
    } else if (read_button(DPAD_UP, CONTROLLER_2)
            && !read_button(DPAD_UP, LAST_INPUT_2)) {
        if (pmenu->cursor == 0) {
            pmenu->cursor = pmenu->size-1;
        } else {
            pmenu->cursor--;
        }
    } else if (read_button(DPAD_DOWN, CONTROLLER_2)
            && !read_button(DPAD_DOWN, LAST_INPUT_2)) {
        if (pmenu->cursor == pmenu->size-1) {
            pmenu->cursor = 0;
        } else {
            pmenu->cursor++;
        }
    } else if (read_button(DPAD_LEFT, CONTROLLER_2)
            && !read_button(DPAD_LEFT, LAST_INPUT_2)) {
        if (pmenu->type[pmenu->cursor] == MENU_VALUE_HWORD) {
            HWORD_T *ptr = (HWORD_T*)pmenu->pvalue[pmenu->cursor];
            *ptr = *ptr - 1;
            // special case for framerate
            if (ptr == FRAME_RATE_1) {
                HWORD_T *ptr2 = (HWORD_T*)FRAME_RATE_2;
                *ptr2 = *ptr;
            }
        } else if (pmenu->type[pmenu->cursor] == MENU_VALUE_WORD) {
            WORD_T *ptr = (WORD_T*)pmenu->pvalue[pmenu->cursor];
            *ptr = *ptr - 1;
        } else if (pmenu->type[pmenu->cursor] == MENU_VALUE_BYTE) {
            BYTE_T *ptr = (BYTE_T*)pmenu->pvalue[pmenu->cursor];
            *ptr = *ptr - 1;
        }
    } else if (read_button(DPAD_RIGHT, CONTROLLER_2)
        && !read_button(DPAD_RIGHT, LAST_INPUT_2)) {
        if (pmenu->type[pmenu->cursor] == MENU_VALUE_HWORD) {
            HWORD_T *ptr = (HWORD_T*)pmenu->pvalue[pmenu->cursor];
            *ptr = *ptr + 1;
            // special case for framerate
            if (ptr == FRAME_RATE_1) {
                HWORD_T *ptr2 = (HWORD_T*)FRAME_RATE_2;
                *ptr2 = *ptr;
            }
        } else if (pmenu->type[pmenu->cursor] == MENU_VALUE_WORD) {
            WORD_T *ptr = (WORD_T*)pmenu->pvalue[pmenu->cursor];
            *ptr = *ptr + 1;
        } else if (pmenu->type[pmenu->cursor] == MENU_VALUE_BYTE) {
            BYTE_T *ptr = (BYTE_T*)pmenu->pvalue[pmenu->cursor];
            *ptr = *ptr + 1;
        }
    }
}
