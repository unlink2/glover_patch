#ifndef __MENU_H__
#define __MENU_H__

#include "typedefs.h"
#include "memory.h"

#define BACK_ACTION 0xFF

typedef struct menudef {
    /*
     * Menu flags
     * 7th bit = 1 -> show menu
     */
    BYTE_T flags;
    BYTE_T cursor; // cursor position

    HWORD_T size; // size of menu strings (max is 0xF)
    char *strings[0xF]; // strings
    // actions for each item
    // if cursor == 0xFFFF -> back action
    void (*pactions)(struct menudef *);
    char *pstr; // string buffer

} menudef;

extern menudef pmenu;

void init_default_menu(menudef *);
void main_menu_select(menudef *);

void render_menu(menudef *);

void update_menu(menudef *);

#endif
