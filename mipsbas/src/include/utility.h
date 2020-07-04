#ifndef __UTILITY_H__
#define __UTILITY_H__

#ifndef NULL
#define NULL ( (void *) 0)
#endif

/**
 * Returns the next part of a string
 * or NULL if string start is \0
 * Size is token's total size until terminator
 */
char* get_next_token(char *pstr, unsigned int *size);

/**
 * No stdlib so we implement some basics here
 */
unsigned int lstrlen(char *pstr);
int lstrncmp(char *ps1, char *ps2, unsigned int max_len);
void lmemcpy(char *psrc, char *pdest, unsigned int size);
void lmemset(char *ptr, unsigned char value, unsigned int size);
void lstrcpy(char *dst, char *src);

#endif
