#include "include/render.h"
#include "include/memwatch.h"

void render() {
    memwatch *pmemwatch = memwatch_from_addr(MEMWATCH_STRUCT);
    render_memwatch(pmemwatch);
}

void puts_xy(char *str, unsigned int x, unsigned int y) {
    void (*pfunction)(WORD_T, char*, WORD_T);
    pfunction = (void (*)(WORD_T, char*, WORD_T))PUTS_XY;

    // set x and y value
    WORD_T *px = PUTS_X_POS;
    WORD_T *py = PUTS_Y_POS;
    *px = x;
    *py = y;

    // TODO find out what arg1 and arg2 are for
    // need to make sure that
    // the call happens exactly in this order
    // with a0 == arg0, a1 == str, a2 == arg2
    // arg2 might be a ptr to a struct used for printing
    pfunction(0x80202240, str, 0x801ED324);
}
