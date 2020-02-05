#include "include/utility.h"
#include "include/logic.h"
#include "include/render.h"

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
            assert(FALSE);
            break;
    }
    return 0;
}





