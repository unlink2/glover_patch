#include "include/inputs.h"

BOOLEAN read_button(unsigned int button, WORD_T *pcontroller) {
    // if the controller is not connected it will
    // return this value
    if (*pcontroller == 0xFFFFFFFF) {
        return 0;
    }
    return (*pcontroller) >> button & 0x01;
}

char get_stick(unsigned char axis, WORD_T *pcontroller) {
    return (*pcontroller) >> (axis*8);
}

void store_inputs(WORD_T *pcontroller, WORD_T *pdest) {
    (*pdest) = (*pcontroller);
}

void clear_last_inputs(WORD_T *pdest) {
    *pdest = 0;
}
