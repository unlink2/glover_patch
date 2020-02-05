#include "include/inputs.h"

BOOLEAN read_button(unsigned int button, unsigned int *controller) {
    return (*controller) >> button & 0x01;
}
