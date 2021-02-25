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


menudef pmenu;

void put_bool(BOOLEAN expr, char *pstr) {
    pstr[1] = expr ? 'x' : ' ';
}

void render_menu(menudef *pmenu) {
    if ((pmenu->flags & 0x80) == 0) {
        return;
    }

    // HWORD_T *pframebuffer = get_frame_buffer();
    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);
    get_ptr(HWORD_T, pfont_hi, FONT8X8_HI, 0x4000);
    // render menu if flag is enabled
    unsigned short start_x = 0x10;
    unsigned short start_y = 0x20;
    // display 16 bytes on screen 1 word per line
    for (int i = 0; i < pmenu->size; i++, start_y += CHAR_H+1) {
        if (i == pmenu->cursor) {
            gputsrdp(pmenu->strings[i], start_x, start_y, pfont_hi);
        } else {
            gputsrdp(pmenu->strings[i], start_x, start_y, pfont);
        }
    }


    // unsigned int x_offset = start_x;
    // unsigned int y_offset = (pmenu->cursor)*(CHAR_H+1)+0x20;

    // render cursor
    /*draw_char('_', pframebuffer, x_offset, y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
    draw_char('_', pframebuffer, x_offset+8, y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);*/

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

