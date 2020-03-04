#include "include/keyboard.h"
#include "include/render.h"
#include "include/font8x8_basic.h"
#include "include/inputs.h"
#include "include/rdp.h"

keyboard pkb;

void init_hex_keyboard(keyboard *pkb) {
    pkb->pcallback = NULL;

    pkb->rows[0] = "01234567";
    pkb->rows[1] = "89ABCDEF";

    pkb->rows_lower[0] = "01234567";
    pkb->rows_lower[1] = "89abcdef";

    pkb->shift = TRUE;
    pkb->row_len = 2;
    pkb->col_len = 8;
    pkb->row = 0;
    pkb->cursor = 0;
    pkb->pinput = NULL;
    pkb->input_index = 0;
    pkb->input_len = 0;

}

void init_keyboard(keyboard *pkb) {
    pkb->pcallback = NULL;

    pkb->rows[0] = "0123456789";
    pkb->rows[1] = "ABCDEFGHIJ";
    pkb->rows[2] = "KLMNOPQRST";
    pkb->rows[3] = "UVWXYZ.,!?";

    pkb->rows_lower[0] = "0123456789";
    pkb->rows_lower[1] = "abcdefghij";
    pkb->rows_lower[2] = "klmnopqrst";
    pkb->rows_lower[3] = "uvwxyz.,!?";

    pkb->shift = TRUE;
    pkb->row_len = 4;
    pkb->col_len = 10;
    pkb->row = 0;
    pkb->cursor = 0;
    pkb->pinput = "Test test";
    pkb->input_index = 0;
    pkb->input_len = 8;
}

void input_request(char *pstr, u32 len, keyboard *pkb, void (*pcallback)(struct keyboard *, void *), void *pdata) {
    pkb->pinput = pstr;
    pkb->pinput[0] = '\0';
    pkb->pcallback = pcallback;
    pkb->pdata = pdata;
    pkb->flags = 0x40;
    pkb->input_len = len;
    pkb->input_index = 0;
}

void update_keyboard(keyboard *pkb) {
    if ((pkb->flags & 0x40) != 0) {
        pkb->flags = 0x80;
        return;
    }

    if ((pkb->flags & 0x80) == 0) {
        return;
    }



    if (read_button(A_INPUT, CONTROLLER_2)
            && !read_button(A_INPUT, LAST_INPUT_2)) {
        if (pkb->input_index < pkb->input_len-1) {
            if (pkb->shift) {
                pkb->pinput[pkb->input_index] = pkb->rows[pkb->row][pkb->cursor];
            } else {
                pkb->pinput[pkb->input_index] = pkb->rows_lower[pkb->row][pkb->cursor];
            }
            pkb->pinput[pkb->input_index+1] = '\0';
            pkb->input_index++;
        }
    } else if (read_button(B_INPUT, CONTROLLER_2)
            && !read_button(B_INPUT, LAST_INPUT_2)) {
        if (pkb->input_index != 0) {
            pkb->input_index--;
            pkb->pinput[pkb->input_index] = '\0';
        } else {
            if (pkb->pcallback) {
                pkb->success = FALSE;
                pkb->pcallback(pkb, pkb->pdata);
            }
            pkb->flags = 0x00;
        }
    } else if (read_button(Z_INPUT, CONTROLLER_2)
            && !read_button(Z_INPUT, LAST_INPUT_2)) {
        if (pkb->pcallback) {
            pkb->success = TRUE;
            pkb->pcallback(pkb, pkb->pdata);
        }
        pkb->flags = 0x00;
    } else if (read_button(L_INPUT, CONTROLLER_2)
            && !read_button(L_INPUT, LAST_INPUT_2)) {
        pkb->shift = !pkb->shift;
    }

    if (read_button(DPAD_UP, CONTROLLER_2)
            && !read_button(DPAD_UP, LAST_INPUT_2)) {
        if (pkb->row == 0) {
            pkb->row = pkb->row_len-1;
        } else {
            pkb->row--;
        }
    } else if (read_button(DPAD_DOWN, CONTROLLER_2)
            && !read_button(DPAD_DOWN, LAST_INPUT_2)) {
        if (pkb->row == pkb->row_len-1) {
            pkb->row = 0;
        } else {
            pkb->row++;
        }
    } else if (read_button(DPAD_LEFT, CONTROLLER_2)
            && !read_button(DPAD_LEFT, LAST_INPUT_2)) {
        if (pkb->cursor == 0) {
            pkb->cursor = pkb->col_len-1;
        } else {
            pkb->cursor--;
        }
    } else if (read_button(DPAD_RIGHT, CONTROLLER_2)
            && !read_button(DPAD_RIGHT, LAST_INPUT_2)) {
        if (pkb->cursor == pkb->col_len-1) {
            pkb->cursor = 0;
        } else {
            pkb->cursor++;
        }
    }
}

void render_keyboard(keyboard *pkb) {
    if ((pkb->flags & 0x80) == 0) {
        return;
    }

    inc_pbuffer(rdp_sync_pipe(get_pbuffer()));

    // render all rows and some instructions
    HWORD_T *pframebuffer = get_frame_buffer();
    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);
    // render if flag is enabled
    unsigned short start_x = 0x18;
    unsigned short start_y = 0x20;

    // render input buffer
    if (pkb->pinput) {
        gputsrdp(pkb->pinput, 0x18, 0x10, pfont);
    }

    if (!pkb->shift) {
        for (int i = 0; i < pkb->row_len; i++, start_y += CHAR_H+1) {
            gputsrdp(pkb->rows_lower[i], start_x, start_y, pfont);
        }
    } else {
        for (int i = 0; i < pkb->row_len; i++, start_y += CHAR_H+1) {
            gputsrdp(pkb->rows[i], start_x, start_y, pfont);
        }
    }

    // render some UI elements
    start_y += CHAR_H*2;
    gputsrdp("A: Select  B: Backspace", start_x, start_y, pfont);
    start_y += CHAR_H+1;
    gputsrdp("Z: Enter   L: Shift", start_x, start_y, pfont);

    unsigned int x_offset = (pkb->cursor)*(CHAR_W)+0x18;
    unsigned int y_offset = (pkb->row)*(CHAR_H+1)+0x20;

    // render cursor
    draw_char('_', pframebuffer, x_offset, y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);

}
