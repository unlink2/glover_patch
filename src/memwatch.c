#include "include/memwatch.h"
#include "include/inputs.h"
#include "include/render.h"
#include "include/font8x8_basic.h"
#include "include/debug.h"

memwatch pmemwatch;
char pmeminput[10]; // input buffer

void init_memwatch(memwatch *pmw) {
    gmemset((BYTE_T*)pmw->watch_addrs, 0x00, MAX_WATCH*sizeof(watch_addr));
    pmw->watch_index = 0;

    get_ptr(BYTE_T, string_buffer, SCREEN_BUFFER, 0x20*0x10);
    pmw->pstr = string_buffer;
    pmw->base_addr = 0x80000000;
    pmw->flags = 0x0;
}

void render_watchaddr(memwatch *pmw) {
    if ((pkb.flags & 0x80)) {
        return;
    }
    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);

    char *pstr = (char*)pmw->pstr;
    u32 start_y = 0x10;
    for (int i = 0; i < MAX_WATCH; i++) {
        if (!pmw->watch_addrs[i].enabled) {
            continue;
        }
        // render strings
        gputsrdp(pmw->watch_addrs[i].name, 0x90, start_y, pfont);
        gputsrdp(pstr, 0x91+gstrlen(pmw->watch_addrs[i].name)*8, start_y, pfont);
        pstr += 0x10;
        start_y += 0x9;
    }
}

void render_watchselect(memwatch *pmw) {
    HWORD_T *pframebuffer = get_frame_buffer();

    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);

    // render strings
    char *pstr = (char*)pmw->pstr;

    for (int i = 0; i < 5; i++) {
        gputsrdp(pstr, 0x10, 0x10+9*i, pfont);
        pstr += 0x10;
    }


    int y_offset = pmw->cursor_pos*9;

    // render memwatch cursor
    draw_char('_', pframebuffer, 0x10, 0x10+y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
    draw_char('_', pframebuffer, 0x18, 0x10+y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
}

void render_memwatch(memwatch *pmw) {
    if (pmw->flags == 0) {
        render_watchaddr(pmw);
        return;
    } else if (pmw->flags & 0x40) {
        render_watchselect(pmw);
        return;
    }

    HWORD_T *pframebuffer = get_frame_buffer();

    get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);

    unsigned short start_x = 0x24;
    unsigned short start_y = 0x20;

    // render strings
    char *pstr = (char*)pmw->pstr;
    gputsrdp(pstr, start_x, start_y-0x10, pfont);
    pstr += 0x10;

    char index_buffer[4];
    // display 16 bytes on screen 1 word per line
    for (int i = 0; i < WORDS_PER_PAGE; i++, start_y += CHAR_H+1, pstr += 0x10) {
        to_hexstr(i*4, index_buffer, 1);
        gputsrdp(index_buffer, start_x-0x18, start_y, pfont);
        gputsrdp(pstr, start_x, start_y, pfont);
    }

    int x_offset = (pmw->cursor_pos%BYTES_PER_LINE)*CHAR_W*2;
    // +1C to offset from address header
    int y_offset = (pmw->cursor_pos/BYTES_PER_LINE)*(CHAR_H+1)+0x10;

    // if cursor is pointing at address
    if (pmw->cursor_pos == 0xFFFF) {
        x_offset = 0;
        y_offset = 0;
    }
    // render memwatch cursor
    draw_char('_', pframebuffer, start_x+x_offset, 0x10+y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
    draw_char('_', pframebuffer, start_x+8+x_offset, 0x10+y_offset,
            (WORD_T*)font8x8_basic, 0xF00F, 0x0000);
}

void prepare_watchaddr(memwatch *pmw) {
    // string buffer
    char *pstr = (char*)pmw->pstr;
    for (int i = 0; i < MAX_WATCH; i++) {
        if (!pmw->watch_addrs[i].enabled) {
            continue;
        }
        switch(pmw->watch_addrs[i].type) {
            case WORD_WATCH:
                to_hexstr((WORD_T)*(WORD_T*)(pmw->watch_addrs[i].paddr), pstr, sizeof(WORD_T));
                break;
            case HWORD_WATCH:
                to_hexstr((HWORD_T)*(HWORD_T*)(pmw->watch_addrs[i].paddr), pstr, sizeof(HWORD_T));
                break;
            case FLOAT_WATCH:
                to_floatstr((float)*(float*)(pmw->watch_addrs[i].paddr), pstr, 10);
                break;
            default:
                to_hexstr((BYTE_T)*(BYTE_T*)(pmw->watch_addrs[i].paddr), pstr, sizeof(BYTE_T));
                break;
        }
        pstr += 0x10;
    }
}

void prepare_watchselect(memwatch *pmw) {
    // set strings
    char *pstr = (char*)pmw->pstr;

    int max_cursor = FLOAT_WATCH;
    if (((WORD_T)pmw->watch_addrs[pmw->watch_index].paddr % sizeof(WORD_T)) == 0) {
        max_cursor = FLOAT_WATCH;
    } else if (((WORD_T)pmw->watch_addrs[pmw->watch_index].paddr % sizeof(HWORD_T)) == 0) {
        max_cursor = HWORD_WATCH;
    } else {
        max_cursor = BYTE_WATCH;
    }

    if (pmw->cursor_pos > max_cursor) {
        pmw->cursor_pos = 0;
    }

    pstr[0] = 'N'; pstr[1] = 'o'; pstr[2] = 'n'; pstr[3] = 'e'; pstr[4] = '\0';
    pstr+= 0x10;
    pstr[0] = 'B'; pstr[1] = 'Y'; pstr[2] = 'T'; pstr[3] = 'E'; pstr[4] = '\0';
    pstr += 0x10;
    pstr[0] = 'H'; pstr[1] = 'W'; pstr[2] = 'O'; pstr[3] = 'R'; pstr[4] = 'D'; pstr[5] = '\0';
    pstr += 0x10;
    pstr[0] = 'W'; pstr[1] = 'O'; pstr[2] = 'R'; pstr[3] = 'D'; pstr[4] = '\0';
    pstr += 0x10;
    pstr[0] = 'F'; pstr[1] = 'l'; pstr[2] = 'o'; pstr[3] = 'a'; pstr[4] = 't'; pstr[5] = '\0';
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
        // if not ascii mode
        if (pmw->flags & 0b00100000) {
            // append ascii representation to hex str
            pstr[0] = ptr[0];
            pstr[1] = ptr[1];
            pstr[2] = ptr[2];
            pstr[3] = ptr[4];
            pstr[4] = '\0';
        } else {
            to_hexstr(*ptr, pstr, sizeof(WORD_T));
        }
    }
}

void update_memwatch(memwatch *pmw) {
    // rollover
    if (pmw->watch_index >= MAX_WATCH) {
        pmw->watch_index = 0;
    }

    // don't test these if memwatch is off
    if ((pmw->flags & 0x40) != 0) {
        // toggle memwatch on start p2
        if (read_button(START_INPUT, CONTROLLER_2)
           && !read_button(START_INPUT, LAST_INPUT_2)) {
            pmw->flags = pmw->flags ^ 0x80;
        }

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
            pmw->watch_addrs[pmw->watch_index].type = pmw->cursor_pos;
            if (pmw->cursor_pos == NO_WATCH) {
                pmw->watch_addrs[pmw->watch_index].enabled = FALSE;
                pmw->flags = 0x00;
                pmw->cursor_pos = 0x00;
                return;
            } else {
                pmw->watch_addrs[pmw->watch_index].enabled = TRUE;
            }
            pmw->flags = 0x00;
            pmw->cursor_pos = 0x00;
            // pmw->watch_index++;
            init_keyboard(&pkb);
            input_request(pmw->watch_addrs[pmw->watch_index].name, 16, &pkb, &watchselect_input_request, pmw);
        }
        return;
    } else if ((pmw->flags & 0x80) == 0) {
        prepare_watchaddr(pmw);
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
    } else if (read_button(DPAD_RIGHT, CONTROLLER_2)
        && !read_button(DPAD_RIGHT, LAST_INPUT_2)) {
        pmw->cursor_pos++;
        if (pmw->cursor_pos >= WORDS_PER_PAGE*BYTES_PER_LINE) {
            pmw->cursor_pos = 0xFFFF;
        }
    } else if (read_button(DPAD_LEFT, CONTROLLER_2)
        && !read_button(DPAD_LEFT, LAST_INPUT_2)) {
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
        WORD_T value;
        if (pmw->cursor_pos == 0xFFFF) {
            value = (WORD_T)GLOVER_ACTOR;
        } else {
            value = (WORD_T)*paddr;
        }

        if ((WORD_T)value >= 0x80000000 && (WORD_T)value < 0x80400000) {
            // follow pointer if in bounds
            pmw->offset = (value-pmw->base_addr)/WORDS_PER_PAGE/sizeof(WORD_T);
        }

    } else if (read_button(B_INPUT, CONTROLLER_2)
            && read_button(B_INPUT, LAST_INPUT_2)) {
        if (pmw->cursor_pos != 0xFFFF) {
            // enable watch for 1 value
            WORD_T *paddr = (WORD_T*)(memwatch_current_addr(pmw)+pmw->cursor_pos);
            pmw->watch_addrs[pmw->watch_index].paddr = paddr;
            // pmw->watch_type = WORD_WATCH;
            pmw->cursor_pos = 0; // set cursor to 0
            pmw->flags = 0x40;
        }

    } else if (read_button(Z_INPUT, CONTROLLER_2)) {
        // clear watch
        // pmw->watch_addr = NULL;
        // pmw->watch_type = NO_WATCH;
        BYTE_T *paddr = memwatch_current_addr(pmw)+pmw->cursor_pos;

        if ((WORD_T)paddr < pmw->base_addr
                || (WORD_T)paddr >= 0x80400000) {
            pmw->offset = 0;
            return;
        }
        pmw->flags = 0x00;
        pmw->pinput_addr = paddr;
        init_hex_keyboard(&pkb);
        input_request(pmeminput, 3, &pkb, &memwatch_input_request, pmw);
    }

    pmw->frame_counter++;
}

void memwatch_input_request(keyboard *pkb, void *pmw) {
    if (!pkb->success) {
        return;
    }

    memwatch *pmemwatch = (memwatch*)pmw;

    *pmemwatch->pinput_addr = from_hexstr(pkb->pinput, gstrlen(pkb->pinput));
}

void watchselect_input_request(keyboard *pkb, void *pmw) {
    memwatch *pmemwatch = (memwatch*)pmw;
    if (!pkb->success) {
        pmemwatch->watch_addrs[pmemwatch->watch_index].name[0] = '\0';
        pmemwatch->watch_index++;
        return;
    }
    pmemwatch->watch_index++;
}

void clear_all_watch(memwatch *pmw) {
    for (int i = 0; i < MAX_WATCH; i++) {
        pmw->watch_addrs[i].enabled = FALSE;
    }
    pmw->watch_index = 0;
}
