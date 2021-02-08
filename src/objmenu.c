#include "objmenu.h"
#include "render.h"
#include "menu.h"
#include "utility.h"
#include "font8x8_basic.h"
#include "inputs.h"
#include "logic.h"
#include "debug.h"
#include "utility.h"
#include "actor.h"
#include "mainmenu.h"

enum OBJ_MENU_ENTRIES {
    ENTRY_NEXT,
    ENTRY_PREV,
    ENTRY_XINC,
    ENTRY_XDEC,
    ENTRY_YINC,
    ENTRY_YDEC,
    ENTRY_ZINC,
    ENTRY_ZDEC,
    ENTRY_LABEL,
    ENTRY_DELTA,
    ENTRY_SHOW_HIDE,
    ENTRY_SAVE_BANK,
    ENTRY_LOAD_BANK,
    ENTRY_SHOW_HIDE_ALL,
    ENTRY_CHANGE_MODEL
};

BYTE_T move_value;
void init_move_menu(menudef *pmenu) {
    pmenu->size = 15;
    pmenu->cursor = 0;
    pmenu->strings[ENTRY_NEXT] = "Next Object";
    pmenu->strings[ENTRY_PREV] = "Prev Object";
    pmenu->strings[ENTRY_XINC] = "X+";
    pmenu->strings[ENTRY_XDEC] = "X-";
    pmenu->strings[ENTRY_YINC] = "Y+";
    pmenu->strings[ENTRY_YDEC] = "Y-";
    pmenu->strings[ENTRY_ZINC] = "Z+";
    pmenu->strings[ENTRY_ZDEC] = "Z-";
    pmenu->strings[ENTRY_LABEL] = "FFFFFFFF"; // is button but use as label only
    pmenu->strings[ENTRY_DELTA] = "+/- Delta    "; // increase delta to move by
    pmenu->strings[ENTRY_SHOW_HIDE] = "Show/Hide";
    pmenu->strings[ENTRY_SAVE_BANK] = "Save Object Bank";
    pmenu->strings[ENTRY_LOAD_BANK] = "Load Object Bank";
    pmenu->strings[ENTRY_SHOW_HIDE_ALL] = "Show/Hide all objects";
    pmenu->strings[ENTRY_CHANGE_MODEL] = "Change Glover Model";
    // pmenu->strings[14] = "ObjBank Savestates";

    pmenu->type[ENTRY_NEXT] = MENU_BUTTON;
    pmenu->type[ENTRY_PREV] = MENU_BUTTON;
    pmenu->type[ENTRY_XINC] = MENU_BUTTON;
    pmenu->type[ENTRY_XDEC] = MENU_BUTTON;
    pmenu->type[ENTRY_YINC] = MENU_BUTTON;
    pmenu->type[ENTRY_YDEC] = MENU_BUTTON;
    pmenu->type[ENTRY_ZINC] = MENU_BUTTON;
    pmenu->type[ENTRY_ZDEC] = MENU_BUTTON;
    pmenu->type[ENTRY_LABEL] = MENU_BUTTON;
    pmenu->type[ENTRY_DELTA] = MENU_VALUE_BYTE;
    pmenu->type[ENTRY_SHOW_HIDE] = MENU_BUTTON;
    pmenu->type[ENTRY_SAVE_BANK] = MENU_BUTTON;
    pmenu->type[ENTRY_LOAD_BANK] = MENU_BUTTON;
    pmenu->type[ENTRY_SHOW_HIDE_ALL] = MENU_BUTTON;
    pmenu->type[ENTRY_CHANGE_MODEL] = MENU_BUTTON;

    // set pvalue 0 to glover pointer
    pmenu->pvalue[ENTRY_NEXT] = GLOVER_ACTOR;
    pmenu->pvalue[ENTRY_DELTA] = &move_value;
    move_value = 1;

    pmenu->pactions = &move_object_select;
    pmenu->pupdate = &move_object_update;
}




void move_object_select(menudef *pmenu) {
    // get actor
    glover_actor *pactor = (glover_actor*)pmenu->pvalue[0];
    BYTE_T *delta = (BYTE_T*)pmenu->pvalue[9];
    get_ptr(glover_actor, pglover, GLOVER_ACTOR, 1);
    get_ptr(camera_t, pcamera, CAMERA_ACTOR, 1);

    switch (pmenu->cursor) {
        case ENTRY_NEXT:
            pmenu->pvalue[0] = pactor->pnext;
            break;
        case ENTRY_PREV:
            pmenu->pvalue[0] = pactor->pprev;
            break;
        case ENTRY_XINC:
            pactor->xpos += *delta;
            break;
        case ENTRY_XDEC:
            pactor->xpos -= *delta;
            break;
        case ENTRY_YINC:
            pactor->ypos += *delta;
            break;
        case ENTRY_YDEC:
            pactor->ypos -= *delta;
            break;
        case ENTRY_ZINC:
            pactor->zpos += *delta;
            break;
        case ENTRY_ZDEC:
            pactor->zpos -= *delta;
            break;
        case ENTRY_LABEL:
            // move glover to object
            pglover->xpos = pactor->xpos;
            pglover->ypos = pactor->ypos - 10.0f; // so we dont clip
            pglover->zpos = pactor->zpos;
            pcamera->xpos = pactor->xpos;
            pcamera->ypos = pactor->ypos;
            pcamera->zpos = pactor->zpos;
            break;
        case ENTRY_SHOW_HIDE: {
            // toggle rendering
            if (pactor->visible_flag) {
                pactor->visible_flag = 0;
            } else {
                pactor->visible_flag = 64;
            }
            break; }
        case ENTRY_SAVE_BANK:
            clone_obj_bank(NULL, MAX_RESTORE_SLOTS+1);
            break;
        case ENTRY_LOAD_BANK:
            restore_actors(NULL, MAX_RESTORE_SLOTS+1);
            break;
        case ENTRY_SHOW_HIDE_ALL:
            toggle_show_objects();
            break;
        /* case 14:
            pmenu->pgpatch->clone_obj_bank = !pmenu->pgpatch->clone_obj_bank;
            if (pmenu->pgpatch->clone_obj_bank) {
                pmenu->strings[14] = "Regulat Savestate";
            } else {
                pmenu->strings[14] = "ObjBank Savestate";
            }
            break; */
        case ENTRY_CHANGE_MODEL: {
            // index glover model
            get_ptr(glover_actor, pglover, GLOVER_ACTOR, 1);
            glover_actor *pact = (glover_actor*)pmenu->pvalue[0];
            if (pact->pmodel_data) {
                pglover->pmodel_data = pact->pmodel_data;
            }

            // TODO extract from ObjBank (Keys ObjBank.mp or TexBank)
            break; }
        default:
            init_default_menu(pmenu);
            break;
    }
}

void move_object_update(menudef *pmenu) {
    // update label
    to_hexstr((WORD_T)pmenu->pvalue[ENTRY_NEXT], pmenu->strings[ENTRY_LABEL], 4);
    BYTE_T *delta = (BYTE_T*)pmenu->pvalue[ENTRY_DELTA];
    to_hexstr((WORD_T)*delta, pmenu->strings[ENTRY_DELTA]+gstrlen("+/- Delta "), 1);
    // get current actor
    glover_actor *pactor = (glover_actor*)pmenu->pvalue[0];
    if (pactor->visible_flag) {
        pmenu->strings[ENTRY_SHOW_HIDE] = "Hide Actor";
    } else {
        pmenu->strings[ENTRY_SHOW_HIDE] = "Show Actor";
    }

    // easy show/hide
    if (read_button(R_INPUT, CONTROLLER_2)
            && !read_button(R_INPUT, LAST_INPUT_2)) {
        u32 select = pmenu->cursor; // store old cursor
        pmenu->cursor = 10; // toggle rendering
        move_object_select(pmenu);
        pmenu->cursor = select; // restore
    }
    // easy model swap
    if (read_button(Z_INPUT, CONTROLLER_2)
            && !read_button(Z_INPUT, LAST_INPUT_2)) {
        u32 select = pmenu->cursor; // store old cursor
        pmenu->cursor = 14; // toggle rendering
        move_object_select(pmenu);
        pmenu->cursor = select; // restore
    }
}
