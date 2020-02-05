#include "include/inputs.h"

BOOLEAN read_button(unsigned int button, WORD_T *pcontroller) {
    return (*pcontroller) >> button & 0x01;
}

void store_inputs(WORD_T *pcontroller, WORD_T *pdest) {
    (*pdest) = (*pcontroller);
}

void clear_last_inputs(WORD_T *pdest) {
    *pdest = 0;
}
