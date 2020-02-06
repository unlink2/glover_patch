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

void word_to_hexstr(WORD_T value, char *pstr) {
    unsigned char shift = 28;
    for (int i = 0; i < 8; i++, shift -= 4) {
         char c = (value >> shift) & 0x0F;
         // to ascii
         if (c < 0xA) {
            c += 0x30;
         } else {
             c += 55;
         }
         pstr[i] = c;
    }
    pstr[8] = '\0';
}
