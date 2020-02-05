#include "include/utility.h"

unsigned int strlen(char *pstr) {
    int i = 0;
    while (pstr[i] != '\0') {
        i++;
    }
    return i;
}

void memcpy(BYTE_T *psrc, BYTE_T *pdest, unsigned int size) {
    for (int i = 0; i < size; i++) {
        pdest[i] = psrc[i];
    }
}

void memset(BYTE_T *ptr, unsigned char value, unsigned int size) {
    for (int i = 0; i < size; i++) {
        ptr[i] = value;
    }
}

void assert(int expression) {
#ifndef NDEBUG
    if (!expression) {
        while (TRUE) {}
    }
#endif 
}
