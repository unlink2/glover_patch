#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "typedefs.h"
#include "memory.h"
#include "utility.h"


typedef struct keyboard {
    // bit 7 == 1 -> enable
    // bit 6 == 1 -> input request started skip a frame then set to 80
    u8 flags;
    BOOLEAN shift;
    BOOLEAN success; // TRUE if input was ok, FALSE is canceled input

    char *pinput; // points to current input request string
    u32 input_len; // max string length
    u32 input_index; // input cursor position

    char *rows[6]; // keyboard layout
    char *rows_lower[6]; // lowercase rows
    u16 row_len; // amount of rows
    u16 col_len; // amount of chars

    u8 row; // cursor position
    u8 cursor;

    // called when input request finishes
    void (*pcallback)(struct keyboard *, void *);
    void *pdata; // callback data
} keyboard;

extern keyboard pkb;

/**
 * Init 0-9 A-F
 */
void init_hex_keyboard(keyboard *);

/**
 * Init 0-9 A-Z
 */
void init_keyboard(keyboard *);

/**
 * Request string input.
 * Callback is called when input is finished
 * Make sure to displaye callees rendering before this call
 * pstr lenght + '\0'
 */
void input_request(char *, u32, keyboard *, void (*)(struct keyboard *, void *), void *);

void update_keyboard(keyboard *);

void render_keyboard(keyboard *);



#endif
