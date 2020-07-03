
#include "include/utility.h"

/**
 * Checks if current char is terminator
 * also can take into account next char
 */
char is_terminator(char c) {
    switch (c) {
        case ' ':
        case '+':
        case '-':
        case '*':
        case '/':
        case '(':
        case ')':
        case '>':
        case '<':
        case '=':
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
