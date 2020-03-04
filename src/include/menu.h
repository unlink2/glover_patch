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
    char *strings[0xF]; // strings
    menu_entry_t type[0xF]; // type of item
    void *pvalue[0xF]; // if menu type is value this is the pointer to the value

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

void init_default_menu(menudef *);
void init_glover_menu(menudef *);

void main_menu_select(menudef *);
void main_menu_update(menudef *);

void glover_menu_update(menudef *);
void glover_menu_select(menudef *);

void render_menu(menudef *);

void update_menu(menudef *);

#endif
