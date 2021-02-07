#ifndef __MENU_H__
#define __MENU_H__

#include "typedefs.h"
#include "memory.h"
#include "memwatch.h"
#include "logic.h"
#include "keyboard.h"

#define BACK_ACTION 0xFF

typedef enum menu_entry_t {
    MENU_BUTTON,
    MENU_VALUE_BYTE,
    MENU_VALUE_HWORD,
    MENU_VALUE_WORD
} menu_entry_t;

typedef struct menudef {
    /*
     * Menu flags
     * 7th bit = 1 -> show menu
     */
    BYTE_T flags;
    BYTE_T cursor; // cursor position

    HWORD_T size; // size of menu strings (max is 0xF)
    char *strings[0x1F]; // strings
    menu_entry_t type[0x1F]; // type of item
    void *pvalue[0x1F]; // if menu type is value this is the pointer to the value

    // actions for each item
    // if cursor == 0xFFFF -> back action
    void (*pactions)(struct menudef *);
    void (*pupdate)(struct menudef *);
    char *pstr; // string buffer

    // usually pointers to global objects
    memwatch *pmemwatch;
    gpatch_t *pgpatch;
    keyboard *pkb;

} menudef;

extern menudef pmenu;

/*
 * Puts a boolean value into the string
 * String in question must start with "[ ]" function will modify
 * contents of those first 3 characters based on the boolean value
 * TODO update all old bool values
 */
void put_bool(BOOLEAN, char *);

void init_glover_menu(menudef *);
void init_move_menu(menudef *);


void glover_menu_update(menudef *);
void glover_menu_select(menudef *);

void move_object_update(menudef *);
void move_object_select(menudef *);

void render_menu(menudef *);

void update_menu(menudef *);

#endif
