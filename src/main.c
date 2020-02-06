#include "include/utility.h"
#include "include/logic.h"
#include "include/render.h"
#include "include/memory.h"

enum START_MODE {
    START_LOGIC,
    START_RENDERING,
    START_DMA
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
    }

    switch (mode) {
        case START_LOGIC:
            logic();
            break;
        case START_RENDERING:
            render();
            break;
        case START_DMA:
            break;
        default:
            gassert(FALSE);
            break;
    }
    return 0;
}





