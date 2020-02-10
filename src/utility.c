#include "include/utility.h"

#ifdef UNIT_TESTS

void* my_malloc(size_t size) {
    void *ptr = malloc(size);
    memset(ptr, 0, size);

    return ptr;
}

void my_free(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
    }
}

#endif

unsigned int gstrlen(char *pstr) {
    int i = 0;
    while (pstr[i] != '\0') {
        i++;
    }
    return i;
}

void gmemcpy(BYTE_T *psrc, BYTE_T *pdest, unsigned int size) {
    for (int i = 0; i < size; i++) {
        pdest[i] = psrc[i];
    }
}

void gmemset(BYTE_T *ptr, unsigned char value, unsigned int size) {
    for (int i = 0; i < size; i++) {
        ptr[i] = value;
    }
}

void gassert(int expression) {
#ifndef NDEBUG
    if (!expression) {
        while (TRUE) {}
    }
#endif
}

void to_hexstr(WORD_T value, char *pstr, WORD_T size) {
    unsigned char shift = size*8-4; // bits to shift
    int i = 0;
    for (i = 0; i < 2*size; i++, shift -= 4) {
         char c = (value >> shift) & 0x0F;
         // to ascii
         if (c < 0xA) {
            c += 0x30;
         } else {
             c += 55;
         }
         pstr[i] = c;
    }
    pstr[i] = '\0';
}
