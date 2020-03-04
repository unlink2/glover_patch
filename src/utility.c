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

void str_reverse(char *pstr, int len) {
    int i = 0, j = len-1, temp;
    while (i < j) {
        temp = pstr[i];
        pstr[i] = pstr[j];
        pstr[j] = temp;
        i++;
        j--;
    }
}

int to_decstr(WORD_T value, char *pstr, WORD_T size) {
    int i = 0;
    while (value) {
        pstr[i] = (value % 10) + '0';
        i++;
        value = value / 10;
    }

    while (i < size) {
        pstr[i++] = '0';
    }
    str_reverse(pstr, i);
    pstr[i] = '\0';
    return i;
}

void to_floatstr(float n, char *pstr, WORD_T size) {
    if ((u32)n == 0xFF800000) {
        pstr[0] = '-';
        pstr[1] = 'I';
        pstr[2] = '\0';
        return;
    } else if ((u32)n == 0x7F800000) {
        pstr[0] = 'I';
        pstr[1] = '\0';
        return;
    } else if (n != n) {
        pstr[0] = 'N';
        pstr[1] = 'a';
        pstr[2] = 'N';
        pstr[3] = '\0';
        return;
    }

    int i = 0;
    if (n < 0) {
        pstr[i] = '-';
        n *= -1;
        i++;
    }


    int in = (int)n; // get int part

    i += to_decstr(in, pstr+i, 0);

    float f = n - (float)in;

    if (size != 0) {
        pstr[i] = '.';
        f = f * (float)gpow(10, size); // amount of numbers after point
        to_decstr((int)f, pstr+i+1, 0);
    }
}

int gpow(int n, int e) {
    if (e == 0) {
        return 1;
    }
    int res = n;
    for (int i = 0; i < e-1; i++) {
        res = res * n;
    }
    return res;
}

int from_hexstr(char *pstr, int len) {
    int result = 0;
    u32 index = (len-1) * 4;
    for (int i = 0; i < len; i++) {
        if (pstr[0] >= '0' && pstr[0] <= '9') {
            result += (pstr[0]-'0') << index;
        } else if (pstr[0] >= 'a' && pstr[0] <= 'f') {
            result += (pstr[0]-'a'+10) << index;
        } else if (pstr[0] >= 'A' && pstr[0] <= 'F') {
            result += (pstr[0]-'A'+10) << index;
        } else {
            return result;
        }

        pstr++;
        index -= 4;
    }
    return result;
}
