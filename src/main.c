#include "utility.h"
#include "logic.h"
#include "render.h"
#include "memory.h"
#include "font8x8_basic.h"
#include "rdp.h"

enum START_MODE {
    START_LOGIC,
    START_RENDERING,
    START_DMA,
    START_INIT
};

/**
 * Start function
 * inputs:
 *  mode = 0 run non-critical code
 *  mode = 1 run rendering code
 *  mode = 2 run dam (not implemented)
 */
int __start(enum START_MODE mode) {
    // test current map id
    BYTE_T *pmap = CURRENT_MAP;

    // if intor cutscene map
    // init memory
    if (*pmap == 0xFF) {
        init_mem();
        if (mode == START_RENDERING) {
            // draw intro message
            // unrolled draw loop to avoid ram usage during init
            HWORD_T *pfb = get_frame_buffer();
            gputs("github.com/unlink2/glover_patch", pfb, 18, 18, (WORD_T*)font8x8_basic, 0xFFFF, 0x000F);
        }
        return 0;
    }

    switch (mode) {
        case START_LOGIC:
            logic();
            break;
        case START_RENDERING:
            // logic();
            render();
            break;
        case START_DMA:
            break;
        case START_INIT:
            init_mem();
            break; 
        default:
            gassert(FALSE);
            break;
    }
    return 0;
}





