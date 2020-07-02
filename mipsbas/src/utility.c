
#include "include/utility.h"

char* get_next_token(char *pstr, unsigned int *size) {
    char *res = pstr;
    *size = 0;

    if (!res) { return NULL; }
    // get rid of leading spaces
    while (res[0] == ' ') {
        res = res + 1;
    }

    char quoted = 0;
    while ((res[*size] != ' ' || quoted) && res[*size] != '\0') {
        if (res[*size] == '"') {
            quoted = !quoted;
        }
        *size = *size + 1;
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
