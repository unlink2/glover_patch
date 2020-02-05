#include "include/utility.h"

unsigned int strlen(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        i++;
    }
    return i;
}

void memcpy(unsigned char *src, unsigned char *dest, unsigned int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = src[i];
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
