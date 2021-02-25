#include "memory.h"
#include "utility.h"
#include "render.h"
#include "font8x8_basic.h"
#include "memwatch.h"
#include "debug.h"
#include "menu.h"
#include "matrix.h"
#include "keyboard.h"
#include "logic.h"
#include "utility.h"
#include "mainmenu.h"

WORD_T *LAST_INPUT_1;
WORD_T *LAST_INPUT_2;
WORD_T last_input1;
WORD_T last_input2;

void init_mem() {
    // last byte of expansion ram
    get_ptr(BYTE_T, plast, EXP_RAM_END, 1);

    last_input1 = 0;
    last_input2 = 0;
    LAST_INPUT_1 = &last_input1;
    LAST_INPUT_2 = &last_input2;

    init_gpatch(&gpatch);

    // zero struct
    gmemset((BYTE_T*)&pmemwatch, 0x00, sizeof(memwatch));
    init_memwatch(&pmemwatch);

    pmenu.flags = 0x00;
    // pass pointers to global state objects
    gmemset((BYTE_T*)&pmenu, 0x00, sizeof(menudef));
    pmenu.pmemwatch = &pmemwatch;
    pmenu.pgpatch = &gpatch;
    pmenu.pkb = &pkb;
    init_default_menu(&pmenu);

    // init projection matrix
    init_vector3(&projection.x, 1, 0, 0);
    init_vector3(&projection.y, 0, 1, 0);
    init_vector3(&projection.z, 1, 0, 1);

    gmemset((BYTE_T*)&pkb, 0x00, sizeof(keyboard));
    init_keyboard(&pkb);
    pkb.render_inputs = FALSE;

    // extend level select
    get_ptr(BYTE_T, ext_level, LEVEL_SELECT, 1);
    *ext_level = 0x30;

    // evd_init();
    // evd_write_msg(0x21);
    pevd_msg = NULL;

    *plast = 0x00;
}
