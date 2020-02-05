#include "include/utility.h"

unsigned int strlen(char *pstr) {
    int i = 0;
    while (pstr[i] != '\0') {
        i++;
    }
    return i;
}

void memcpy(unsigned char *psrc, unsigned char *pdest, unsigned int size) {
    for (int i = 0; i < size; i++) {
        pdest[i] = psrc[i];
    }
}

void memset(unsigned char *ptr, unsigned char value, unsigned int size) {
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
