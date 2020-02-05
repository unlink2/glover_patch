#include "include/inputs.h"

BOOLEAN read_button(unsigned int button, unsigned int *pcontroller) {
    return (*pcontroller) >> button & 0x01;
}

void store_inputs(unsigned int *pcontroller, unsigned int *pdest) {
    (*pdest) = (*pcontroller);
}

void clear_last_inputs(unsigned int *pdest) {
    *pdest = 0;
}
