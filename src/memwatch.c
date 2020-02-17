#include "include/memwatch.h"
#include "include/inputs.h"
#include "include/render.h"
#include "include/font8x8_basic.h"

memwatch pmemwatch;

void init_memwatch(memwatch *pmw) {
    get_ptr(BYTE_T, string_buffer, SCREEN_BUFFER, 0x20*0x10);
    pmw->pstr = string_buffer;
    pmw->base_addr = 0x80000000;
    pmw->flags = 0x0;
}

void render_watchaddr(memwatch *pmw) {
    HWORD_T *pframebuffer = get_frame_buffer();

    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);

    // render strings
    char *pstr = (char*)pmw->pstr;
    gputsf(pstr, pframebuffer, 0x04, 0x04, pfont);
}

void render_watchselect(memwatch *pmw) {
    HWORD_T *pframebuffer = get_frame_buffer();

    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);

    // render strings
    char *pstr = (char*)pmw->pstr;

    gputsf(pstr, pframebuffer, 0x04, 0x04, pfont);
    pstr += 0x10;

    gputsf(pstr, pframebuffer, 0x04, 0x04+9, pfont);
    pstr += 0x10;

    gputsf(pstr, pframebuffer, 0x04, 0x04+18, pfont);

    int y_offset = pmw->cursor_pos*9;

    // render memwatch cursor
    draw_char('_', pframebuffer, 0x04, 0x04+y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
    draw_char('_', pframebuffer, 0x0C, 0x04+y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
}

void render_memwatch(memwatch *pmw) {
    if (pmw->flags == 0) {
        if (pmw->watch_addr != NULL && pmw->watch_type != NO_WATCH) {
            render_watchaddr(pmw);
        }
        return;
    } else if (pmw->flags & 0x40) {
        render_watchselect(pmw);
        return;
    }

    HWORD_T *pframebuffer = get_frame_buffer();

    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);

    // render strings
    char *pstr = (char*)pmw->pstr;
    gputsf(pstr, pframebuffer, 0x04, 0x04, pfont);
    pstr += 0x10;


    unsigned short start_x = 0x04;
    unsigned short start_y = 0x020;
    // display 16 bytes on screen 1 word per line
    for (int i = 0; i < WORDS_PER_PAGE; i++, start_y += CHAR_H+1, pstr += 0x10) {
        gputsf(pstr, pframebuffer, start_x, start_y, pfont);
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

void prepare_watchaddr(memwatch *pmw) {
    // string buffer
    char *pstr = (char*)pmw->pstr;
    switch(pmw->watch_type) {
        case WORD_WATCH:
            to_hexstr((WORD_T)*pmw->watch_addr, pstr, sizeof(WORD_T));
            break;
        case HWORD_WATCH:
            to_hexstr((HWORD_T)*(HWORD_T*)(pmw->watch_addr), pstr, sizeof(HWORD_T));
            break;
        default:
            to_hexstr((BYTE_T)*(BYTE_T*)(pmw->watch_addr), pstr, sizeof(BYTE_T));
            break;
    }
}

void prepare_watchselect(memwatch *pmw) {
    // set strings
    char *pstr = (char*)pmw->pstr;

    int max_cursor = WORD_WATCH;
    if (((WORD_T)pmw->watch_addr % sizeof(WORD_T)) == 0) {
        max_cursor = WORD_WATCH-1;
    } else if (((WORD_T)pmw->watch_addr % sizeof(HWORD_T)) == 0) {
        max_cursor = HWORD_WATCH-1;
    } else {
        max_cursor = BYTE_WATCH-1;
    }

    if (pmw->cursor_pos > max_cursor) {
        pmw->cursor_pos = 0;
    }
    

    pstr[0] = 'B'; pstr[1] = 'Y'; pstr[2] = 'T'; pstr[3] = 'E'; pstr[4] = '\0';
    pstr += 0x10;
    pstr[0] = 'H'; pstr[1] = 'W'; pstr[2] = 'O'; pstr[3] = 'R'; pstr[4] = 'D'; pstr[5] = '\0';
    pstr += 0x10;
    pstr[0] = 'W'; pstr[1] = 'O'; pstr[2] = 'R'; pstr[3] = 'D'; pstr[4] = '\0';
}

void prepare_memwatch(memwatch *pmw) {
    WORD_T *paddr = (WORD_T*)(memwatch_current_addr(pmw));

    if ((WORD_T)paddr < pmw->base_addr
            || (WORD_T)paddr >= 0x80400000) {
        pmw->offset = 0;
        return;
    }
    // string buffer
    char *pstr = (char*)pmw->pstr;
    to_hexstr((WORD_T)paddr, pstr, sizeof(WORD_T));
    pstr += 0x10;

    // display 16 bytes on screen 1 word per line
    for (int i = 0; i < WORDS_PER_PAGE; i++, pstr += 0x10) {
        WORD_T *ptr = (WORD_T*)paddr+i;
        to_hexstr(*ptr, pstr, sizeof(WORD_T));
    }
}

void update_memwatch(memwatch *pmw) {
    // toggle memwatch on start p2
    if (read_button(START_INPUT, CONTROLLER_2)
            && !read_button(START_INPUT, LAST_INPUT_2)) {
        pmw->flags = pmw->flags ^ 0x80;
    }

    // don't test these if memwatch is off
    if ((pmw->flags & 0x40) != 0) {
        prepare_watchselect(pmw);

        // watch select specific update inputs
        if (read_button(DPAD_UP, CONTROLLER_2)
        && !read_button(DPAD_UP, LAST_INPUT_2)) {
            pmw->cursor_pos--;
        } else if (read_button(DPAD_DOWN, CONTROLLER_2)
        && !read_button(DPAD_DOWN, LAST_INPUT_2)) {
            pmw->cursor_pos++;
        } else if (read_button(A_INPUT, CONTROLLER_2)
                && read_button(A_INPUT, LAST_INPUT_2)) {
            // select type
            pmw->watch_type = pmw->cursor_pos+1;
            pmw->flags = 0x00;
            pmw->cursor_pos = 0x00;
        }
        return;
    } else if ((pmw->flags & 0x80) == 0) {
        if (pmw->watch_addr != NULL && pmw->watch_type != NO_WATCH) {
            prepare_watchaddr(pmw);
        }
        return;
    }

    // normal update code
    // don't go here during watchselect 

    if ((pmw->frame_counter % MEMWATCH_POLLING_RATE) == 0) {
        // don't update every frame
        prepare_memwatch(pmw);
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
            BYTE_T *paddr = memwatch_current_addr(pmw)+pmw->cursor_pos;

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
            BYTE_T *paddr = memwatch_current_addr(pmw)+pmw->cursor_pos;

            if ((WORD_T)paddr < pmw->base_addr
                    || (WORD_T)paddr >= 0x80400000) {
                pmw->offset = 0;
                return;
            }
            *paddr -= 1;
        }
    } else if (read_button(A_INPUT, CONTROLLER_2)
            && read_button(A_INPUT, LAST_INPUT_2)) {
        // get current address and read it
        WORD_T *paddr = (WORD_T*)(memwatch_current_addr(pmw)+pmw->cursor_pos);

        // get start of word
        paddr = (WORD_T*)((WORD_T)paddr & ~3);
        // read value and verify it is in range
        WORD_T value = (WORD_T)*paddr;

        if ((WORD_T)value >= 0x80000000 && (WORD_T)value < 0x80400000) {
            // follow pointer if in bounds
            pmw->offset = (value-pmw->base_addr)/WORDS_PER_PAGE/sizeof(WORD_T);
        }

    } else if (read_button(B_INPUT, CONTROLLER_2)
            && read_button(B_INPUT, LAST_INPUT_2)) {
        if (pmw->cursor_pos != 0xFFFF) {
            // enable watch for 1 value
            WORD_T *paddr = (WORD_T*)(memwatch_current_addr(pmw)+pmw->cursor_pos);
            pmw->watch_addr = paddr;
            // pmw->watch_type = WORD_WATCH;
            pmw->cursor_pos = 0; // set cursor to 0
            pmw->flags = 0x40;
        }

    } else if (read_button(Z_INPUT, CONTROLLER_2)) {
        // clear watch
        pmw->watch_addr = NULL;
        pmw->watch_type = NO_WATCH;
    }

    pmw->frame_counter++;
}
