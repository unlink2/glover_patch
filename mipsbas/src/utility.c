
#include "include/utility.h"

/**
 * Checks if current char is terminator
 * also can take into account next char
 */
char is_terminator(char c) {
    // easier to read than a straight up return if
    switch (c) {
        case ' ':
        case '(':
        case ')':
        case ',':
            return 1;
    }
    return 0;
}

char* get_next_token(char *pstr, unsigned int *size) {
    char *res = pstr;
    *size = 0;

    if (!res) { return NULL; }
    // get rid of leading spaces
    while (res[0] == ' ') {
        res = res + 1;
    }

    // if first char is terminator other than space increment by 1 but include it in token
    if (is_terminator(res[0]) && is_terminator(res[1]) && res[1] != ' ') {
        *size = *size + 1;
    }

    char quoted = 0;
    while ((!is_terminator(res[*size]) || quoted) && res[*size] != '\0') {
        if (res[*size] == '"') {
            quoted = !quoted;
        }
        *size = *size + 1;
    }

    // if 2 terminators follow each other it is likely an operator like == or <>
    if (is_terminator(res[0]) && is_terminator(res[1]) && *size == 1) {
       *size = *size + 1;
    } else if (is_terminator(res[0]) && *size == 0) {
        // single terminator token
        *size = 1;
    }

    // return NULL if quote did not terminate
    if (res[*size] == '\0' && quoted) {
        return NULL;
    }

    if (*size == 0) {
        return NULL;
    }
    return res;
}

unsigned int lstrlen(char *pstr) {
    int i = 0;
    while (pstr[i] != '\0') {
        i++;
    }
    return i;
}

int lstrncmp(char *ps1, char *ps2, unsigned int max_len) {
    int diff = 0;
    unsigned int len1 = lstrlen(ps1);
    unsigned int len2 = lstrlen(ps2);
    int i;
    for (i = 0; i < len1 && i < max_len; i++) {
        if (ps1[i] != ps2[i]) {
            diff++;
        }
    }

    if (len1 != len2 && i < max_len) {
        return len1-len2;
    }

    return diff;
}

void lmemcpy(char *psrc, char *pdest, unsigned int size) {
    for (int i = 0; i < size; i++) {
        pdest[i] = psrc[i];
    }
}

void lmemset(char *ptr, unsigned char value, unsigned int size) {
    for (int i = 0; i < size; i++) {
        ptr[i] = value;
    }
}

void lstrcpy(char *dst, char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}
