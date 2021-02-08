#include "mainmenu.h"
#include "render.h"
#include "menu.h"
#include "utility.h"
#include "font8x8_basic.h"
#include "inputs.h"
#include "logic.h"
#include "debug.h"
#include "utility.h"
#include "actor.h"
#include "othermenu.h"
#include "objmenu.h"

enum MAIN_MENU_ENTRIES {
    ENTRY_MEM_MON = 0,
    ENTRY_ASCII_MON,
    ENTRY_SAVE_POS,
    ENTRY_LOAD_POS,
    ENTRY_SAVE_ACT,
    ENTRY_LOAD_ACT,
    ENTRY_TOGGLE_TIMER,
    ENTRY_LEVEL_SEL,
    ENTRY_TOGGLE_COL,
    ENTRY_FOG,
    ENTRY_OTHER,
    ENTRY_FPS,
    ENTRY_INIT_ED,
    ENTRY_CLEAR_WATCH,
    ENTRY_MOVE_OBJ
};

void init_default_menu(menudef *pmenu) {
    get_ptr(char, string_buffer, SCREEN_BUFFER, 0x20*0x10);
    pmenu->pstr = string_buffer;
    pmenu->size = 15;
    pmenu->cursor = 0;
    pmenu->strings[ENTRY_MEM_MON] = "Memory Monitor";
    pmenu->strings[ENTRY_ASCII_MON] = "Memory Monitor ASCII";
    pmenu->strings[ENTRY_SAVE_POS] = "Save Position";
    pmenu->strings[ENTRY_LOAD_POS] = "Load Position";
    pmenu->strings[ENTRY_SAVE_ACT] = "Save Actors     ";
    pmenu->strings[ENTRY_LOAD_ACT] = "Load Actors     ";
    pmenu->strings[ENTRY_TOGGLE_TIMER] = "Toggle Timer";
    pmenu->strings[ENTRY_LEVEL_SEL] = "Level Select";
    pmenu->strings[ENTRY_TOGGLE_COL] = "Toggle Collision";
    pmenu->strings[ENTRY_FOG] = "Fog";
    pmenu->strings[ENTRY_OTHER] = "Other...";
    pmenu->strings[ENTRY_FPS] = "FPS:            ";
    pmenu->strings[ENTRY_INIT_ED] = "Init ED...";
    pmenu->strings[ENTRY_CLEAR_WATCH] = "Clear watch...";
    pmenu->strings[ENTRY_MOVE_OBJ] = "Move Object...";

    pmenu->type[ENTRY_MEM_MON] = MENU_BUTTON;
    pmenu->type[ENTRY_ASCII_MON] = MENU_BUTTON;
    pmenu->type[ENTRY_SAVE_POS] = MENU_BUTTON;
    pmenu->type[ENTRY_LOAD_POS] = MENU_BUTTON;
    pmenu->type[ENTRY_SAVE_ACT] = MENU_VALUE_HWORD;
    pmenu->type[ENTRY_LOAD_ACT] = MENU_VALUE_HWORD;
    pmenu->type[ENTRY_TOGGLE_TIMER] = MENU_BUTTON;
    pmenu->type[ENTRY_LEVEL_SEL] = MENU_BUTTON;
    pmenu->type[ENTRY_TOGGLE_COL] = MENU_BUTTON;
    pmenu->type[ENTRY_FOG] = MENU_BUTTON;
    pmenu->type[ENTRY_OTHER] = MENU_BUTTON;
    pmenu->type[ENTRY_FPS] = MENU_VALUE_HWORD;
    pmenu->type[ENTRY_INIT_ED] = MENU_BUTTON;
    pmenu->type[ENTRY_CLEAR_WATCH] = MENU_BUTTON;
    pmenu->type[ENTRY_MOVE_OBJ] = MENU_BUTTON;

    pmenu->pvalue[ENTRY_SAVE_ACT] = &pmenu->pgpatch->restore_slot;
    pmenu->pvalue[ENTRY_LOAD_ACT] = &pmenu->pgpatch->restore_slot;
    pmenu->pvalue[ENTRY_FPS] = (void*)FRAME_RATE_1;

    pmenu->pactions = &main_menu_select;
    pmenu->pupdate = &main_menu_update;
}

void main_menu_select(menudef *pmenu) {
    switch(pmenu->cursor) {
        case ENTRY_MEM_MON:
            pmenu->pmemwatch->flags = pmenu->pmemwatch->flags ^ 0x80;
            // not ascii mode
            pmenu->pmemwatch->flags = pmenu->pmemwatch->flags & 0b11011111;
            pmenu->flags = 0x00;
            break;
        case ENTRY_ASCII_MON:
            // ascii mode
            pmenu->pmemwatch->flags = pmenu->pmemwatch->flags ^ 0b10100000;
            pmenu->flags = 0x00;
            break;
        case ENTRY_SAVE_POS:
            store_glover_pos();
            break;
        case ENTRY_LOAD_POS:
            restore_glover_pos();
            break;
        case ENTRY_SAVE_ACT:
            clone_actors(NULL, pmenu->pgpatch->restore_slot);
            break;
        case ENTRY_LOAD_ACT:
            restore_actors(NULL, pmenu->pgpatch->restore_slot);
            break;
        case ENTRY_TOGGLE_TIMER:
            // enable_timer();
            toggle_timer(pmenu->pgpatch);
            break;
        case ENTRY_LEVEL_SEL:
            level_select();
            break;
        case ENTRY_TOGGLE_COL:
            toggle_collision();
            break;
        case ENTRY_FOG:
            toggle_fog();
            break;
        case ENTRY_OTHER:
            init_glover_menu(pmenu);
            break;
        case ENTRY_INIT_ED:
            evd_init();
            break;
        case ENTRY_CLEAR_WATCH:
            clear_all_watch(pmenu->pmemwatch);
            break;
        case ENTRY_MOVE_OBJ:
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
    HWORD_T *ptr = (HWORD_T*)pmenu->pvalue[ENTRY_FPS];
    char *str = pmenu->strings[ENTRY_FPS];
    to_hexstr(*ptr, str+5, 2);

    pmenu->pgpatch->restore_slot = pmenu->pgpatch->restore_slot & MAX_RESTORE_SLOTS;
    // restore to hex
    to_hexstr((WORD_T)pmenu->pgpatch->restore_slot, pmenu->strings[ENTRY_LOAD_ACT]+gstrlen("Load Actors "), 1);
    to_hexstr((WORD_T)pmenu->pgpatch->restore_slot, pmenu->strings[ENTRY_SAVE_ACT]+gstrlen("Save Actors "), 1);
}

