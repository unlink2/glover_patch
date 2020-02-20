#include "include/render.h"
#include "include/menu.h"
#include "include/utility.h"
#include "include/font8x8_basic.h"
#include "include/inputs.h"
#include "include/memwatch.h"
#include "include/logic.h"

menudef pmenu;

void init_default_menu(menudef *pmenu) {
    get_ptr(char, string_buffer, SCREEN_BUFFER, 0x20*0x10);
    pmenu->pstr = string_buffer;
    pmenu->size = 9;
    pmenu->cursor = 0;
    pmenu->strings[0] = "Memory Monitor";
    pmenu->strings[1] = "Save Position";
    pmenu->strings[2] = "Load Position";
    pmenu->strings[3] = "Save Actors";
    pmenu->strings[4] = "Load Actors";
    pmenu->strings[5] = "Start Timer";
    pmenu->strings[6] = "Level Select";
    pmenu->strings[7] = "Toggle Collision";
    pmenu->strings[8] = "Fog";

    pmenu->type[0] = MENU_BUTTON;
    pmenu->type[1] = MENU_BUTTON;
    pmenu->type[2] = MENU_BUTTON;
    pmenu->type[3] = MENU_BUTTON;
    pmenu->type[4] = MENU_BUTTON;
    pmenu->type[5] = MENU_BUTTON;
    pmenu->type[6] = MENU_BUTTON;
    pmenu->type[7] = MENU_BUTTON;
    pmenu->type[8] = MENU_BUTTON;

    pmenu->pactions = &main_menu_select;
    pmenu->pupdate = &main_menu_update;
}

void main_menu_select(menudef *pmenu) {
    switch(pmenu->cursor) {
        case 0:
            pmemwatch.flags = pmemwatch.flags ^ 0x80;
            pmenu->flags = 0x00;
            break;
        case 1:
            store_glover_pos();
            break;
        case 2:
            restore_glover_pos();
            break;
        case 3:
            clone_actors();
            break;
        case 4:
            restore_actors();
            break;
        case 5:
            enable_timer();
            break;
        case 6:
            level_select();
            break;
        case 7:
            toggle_collision();
            break;
        case 8:
            toggle_fog();
            break;
        default:
            pmenu->flags = 0x00;
            pmenu->cursor = 0;
            break;
    }
}

void main_menu_update(menudef *pmenu) {
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
        gputsf(pmenu->strings[i], pframebuffer, start_x, start_y, pfont);
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
    }

}
