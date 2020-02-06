#include "include/memory.h"
#include "include/utility.h"

void init_mem() {
    get_ptr(BYTE_T, pframe_advance, FRAME_ADVANCE);
    *pframe_advance = 0x00;
}
