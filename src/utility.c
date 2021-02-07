#include "utility.h"

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

int gstrncmp(char *ps1, char *ps2, unsigned int max_len) {
    int diff = 0;
    unsigned int len1 = gstrlen(ps1);
    unsigned int len2 = gstrlen(ps2);
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

void *gmemcpy(void *src, void *dest, unsigned int n) {
    return _gmemcpy(dest, src, n);
}

void *_gmemcpy(void *dest, void *src, unsigned int n) {
    char *dp = dest;
    const char *sp = src;
    while (n--) {
        *dp++ = *sp++;
    }
    return dest;
}

void gmemset(BYTE_T *ptr, unsigned char value, unsigned int size) {
    for (int i = 0; i < size; i++) {
        ptr[i] = value;
    }
}

void gstrcpy(char *dst, char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
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

void to_hexstr_signed(WORD_T value, char *pstr, WORD_T size) {
    unsigned char shift = size*8-4; // bits to shift
    int i = 0;
    int str_index = 0;
    for (i = 0; i < 2*size; i++, str_index++, shift -= 4) {
        char c = (value >> shift) & 0x0F;
        // check sign if 0
        if (i == 0) {
            if (c & 0x8) {
                pstr[str_index] = '-';
                // adjust value 2s complement
                value = (~value) + 1;;
                c = (value >> shift) & 0x0F;
                str_index++;
            }
        }

         // to ascii
         if (c < 0xA) {
            c += 0x30;
         } else {
             c += 55;
         }
         pstr[str_index] = c;
    }
    pstr[str_index] = '\0';
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
    int org_value = value;
    while (value) {
        pstr[i] = (value % 10) + '0';
        i++;
        value = value / 10;
    }

    while (i < size) {
        pstr[i++] = '0';
    }

    if (org_value < 0) {
        pstr[i] = '-';
        i++;
    }

    str_reverse(pstr, i);
    pstr[i] = '\0';
    return i;
}

void to_floatstr(float n, char *pdest, WORD_T size) {
    to_floatstr_pad(n, pdest, size, 0);
}

void to_floatstr_pad(float n, char *pstr, WORD_T size, BYTE_T pad) {
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

    // pad float to exact amount of characters
    // using \1 char to make it 'invisible'
    for (int i = gstrlen(pstr); i < pad; i++) {
        pstr[i] = 1;
        pstr[i+1] = '\0';
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

WORD_T from_hexstr(char *pstr, int len) {
    int result = 0;
    u32 index = (len-1) * 4;
    for (int i = 0; i < len; i++) {
        if (pstr[0] == '\0') {
            break;
        }
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

void split_space(char *pstr, char **head, char **tail) {
    unsigned int len = gstrlen(pstr);
    *head = pstr;
    *tail = NULL;
    for (int i = 0; i < len; i++) {
        if (pstr[i] == ' ') {
            pstr[i] = '\0';
            if (i+1 < len) {
                *tail = pstr+i+1; // next string
            } else {
                *tail = NULL;
            }
            break;
        }
    }
}

arg parse_arg(char *parg, const char *pkey) {
    arg a;

    a.key = pkey; //first_part
    // if it is arg then the size must fit
    if (is_arg(parg, pkey)) {
        a.value = parg+gstrlen((char*)pkey);
    } else {
        a.value = NULL;
        a.key = NULL;
    }

    return a;
}

BOOLEAN is_arg(char *pa, const char *pkey) {
    return gstrncmp(pa, (char*)pkey, gstrlen((char*)pkey)) == 0;
}



