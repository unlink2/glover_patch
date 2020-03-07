#include "include/memory.h"
#include "include/utility.h"
#include "include/render.h"
#include "include/font8x8_basic.h"
#include "include/memwatch.h"
#include "include/debug.h"
#include "include/menu.h"
#include "include/matrix.h"
#include "include/keyboard.h"
#include "include/logic.h"
#include "include/utility.h"

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

    // zero all ram

    gmemset((BYTE_T*)&gpatch, 0x00, sizeof(gpatch));
    gpatch.frame_advance = 0x00;
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

    // init some data
    // TODO
    /*get_ptr(HWORD_T, pfont, FONT8X8, 0x4000);
    get_ptr(WORD_T, pbuffer, RDP_DL_BUFFER, RDP_DL_SIZE);
    get_ptr(WORD_T, pbuffer_memwatch, RDP_DL_BUFFER_MEMWATCH, RDP_DL_SIZE);
    get_ptr(WORD_T, pbuffer_keyboard, RDP_DL_BUFFER_KEYBOARD, RDP_DL_SIZE);
    if (plast != 0) {
        decompress_font((WORD_T*)font8x8_basic, pfont, 0x000F, 0xFFFF);
        // clear rdp buffer once as well
        gmemset((BYTE_T*)pbuffer, 0x00, RDP_DL_SIZE*sizeof(WORD_T));
        gmemset((BYTE_T*)pbuffer_memwatch, 0x00, RDP_DL_SIZE*sizeof(WORD_T));
        gmemset((BYTE_T*)pbuffer_keyboard, 0x00, RDP_DL_SIZE*sizeof(WORD_T));

    }*/

    // evd_init();
    // evd_write_msg(0x21);
    pevd_msg = NULL;

    *plast = 0x00;
}
