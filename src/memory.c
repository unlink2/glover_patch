#include "include/memory.h"
#include "include/utility.h"
#include "include/render.h"
#include "include/font8x8_basic.h"
#include "include/memwatch.h"
#include "include/debug.h"
#include "include/menu.h"

void init_mem() {
    get_ptr(BYTE_T, pframe_advance, FRAME_ADVANCE, 1);
    *pframe_advance = 0x00;
    init_memwatch(&pmemwatch);
    pmenu.flags = 0x00;
    init_default_menu(&pmenu);

    // evd_init();
    // evd_write_msg(0x21);
}
