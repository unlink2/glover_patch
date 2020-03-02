#include "include/memory.h"
#include "include/utility.h"
#include "include/render.h"
#include "include/font8x8_basic.h"
#include "include/memwatch.h"
#include "include/debug.h"
#include "include/menu.h"
#include "include/matrix.h"

void init_mem() {
    get_ptr(BYTE_T, pframe_advance, FRAME_ADVANCE, 1);
    *pframe_advance = 0x00;
    init_memwatch(&pmemwatch);

    pmenu.flags = 0x00;
    // pass pointers to global state objects
    pmenu.pmemwatch = &pmemwatch;
    pmenu.pgpatch = &gpatch;
    init_default_menu(&pmenu);

    // init projection matrix
    init_vector3(&projection.x, 1, 0, 0);
    init_vector3(&projection.y, 0, 1, 0);
    init_vector3(&projection.z, 1, 0, 1);

    // clear rdp buffer
    get_ptr(WORD_T, pbuffer, RDP_DL_BUFFER, 0x500);
    gmemset((BYTE_T*)pbuffer, 0x00, 0x500*sizeof(WORD_T));

    // evd_init();
    // evd_write_msg(0x21);
}
