#include "include/memwatch.h"
#include "include/inputs.h"
#include "include/render.h"
#include "include/font8x8_basic.h"

memwatch* memwatch_from_addr(WORD_T *paddress) {
    get_ptr(memwatch, newwatch, paddress, sizeof(memwatch));
    get_ptr(BYTE_T, string_buffer, STRING_BUFFER, 20);
    newwatch->pstr = string_buffer;
    newwatch->base_addr = 0x80000000;
    return newwatch;
}

void render_memwatch(memwatch *pmw) {
    if ((pmw->flags & 0b1000000000000000) == 0) {
        return;
    }

    HWORD_T *pframebuffer = get_frame_buffer();

    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);


    WORD_T *paddr = (WORD_T*)pmw->base_addr+(pmw->offset*WORDS_PER_PAGE);

    if ((WORD_T)paddr < pmw->base_addr
            || (WORD_T)paddr >= 0x80400000) {
        pmw->offset = 0;
        return;
    }

    word_to_hexstr((WORD_T)paddr, (char*)pmw->pstr);
    gputsf((char*)pmw->pstr, pframebuffer, 0x04, 0x04, pfont);

    // display 16 bytes on screen 2 words per line
    unsigned short start_x = 0x04;
    unsigned short start_y = 0x020;
    for (int i = 0; i < WORDS_PER_PAGE; i++, start_y += CHAR_H+1) {
        WORD_T *ptr = (WORD_T*)paddr+i;
        word_to_hexstr(*ptr, (char*)pmw->pstr);
        gputsf((char*)pmw->pstr, pframebuffer, start_x, start_y, pfont);

    }

    int x_offset = (pmw->cursor_pos%BYTES_PER_LINE)*CHAR_W*2;
    // +1C to offset from address header
    int y_offset = (pmw->cursor_pos/BYTES_PER_LINE)*(CHAR_H+1)+0x1C;

    // if cursor is pointing at address
    if (pmw->cursor_pos == 0xFFFF) {
        x_offset = 0;
        y_offset = 0;
    }
    // render memwatch cursor
    draw_char('_', pframebuffer, 0x04+x_offset, 0x04+y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
    draw_char('_', pframebuffer, 0x0C+x_offset, 0x04+y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
}

void update_memwatch(memwatch *pmw) {
    // toggle memwatch on start p2
    if (read_button(START_INPUT, CONTROLLER_2)
            && !read_button(START_INPUT, LAST_INPUT_2)) {
        pmw->flags = pmw->flags ^ 0b1000000000000000;
    }

    // don't test these if memwatch is off
    if ((pmw->flags & 0x8000) == 0) {
        return;
    }

    // inc/dec memory offset
    if (read_button(L_INPUT, CONTROLLER_2)) {
        pmw->offset += 0x100;
    } else if (read_button(R_INPUT, CONTROLLER_2)) {
        pmw->offset -= 0x100;
    } else if (read_button(DPAD_LEFT, CONTROLLER_2)
        && !read_button(DPAD_LEFT, LAST_INPUT_2)) {
        pmw->cursor_pos++;
        if (pmw->cursor_pos >= WORDS_PER_PAGE*BYTES_PER_LINE) {
            pmw->cursor_pos = 0xFFFF;
        }
    } else if (read_button(DPAD_RIGHT, CONTROLLER_2)
        && !read_button(DPAD_RIGHT, LAST_INPUT_2)) {
        pmw->cursor_pos--;
        if (pmw->cursor_pos >= WORDS_PER_PAGE*BYTES_PER_LINE
                && pmw->cursor_pos != 0xFFFF) {
            pmw->cursor_pos = WORDS_PER_PAGE*BYTES_PER_LINE-1;
        }
    } else if (read_button(DPAD_UP, CONTROLLER_2)
        && !read_button(DPAD_UP, LAST_INPUT_2)) {
        // if cursor is pointing at address
        if (pmw->cursor_pos == 0xFFFF) {
            pmw->offset++;
        } else {
            BYTE_T *paddr = (BYTE_T*)(pmw->base_addr+(pmw->offset*WORDS_PER_PAGE*sizeof(WORD_T)))+pmw->cursor_pos;

            if ((WORD_T)paddr < pmw->base_addr
                    || (WORD_T)paddr >= 0x80400000) {
                pmw->offset = 0;
                return;
            }
            *paddr += 1;
        }
    } else if (read_button(DPAD_DOWN, CONTROLLER_2)
        && !read_button(DPAD_DOWN, LAST_INPUT_2)) {
        // if cursor is pointing at address
        if (pmw->cursor_pos == 0xFFFF) {
            pmw->offset--;
        } else {
            BYTE_T *paddr = (BYTE_T*)pmw->base_addr+(pmw->offset*WORDS_PER_PAGE*sizeof(WORD_T))+pmw->cursor_pos;

            if ((WORD_T)paddr < pmw->base_addr
                    || (WORD_T)paddr >= 0x80400000) {
                pmw->offset = 0;
                return;
            }
            *paddr -= 1;
        }
    }
}
