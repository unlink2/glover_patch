#ifndef __UTILITY_H__
#define __UTILITY_H__

#ifndef NULL
#define NULL ( (void *) 0)
#endif

/**
 * Returns the next part of a string
 * or NULL if string start is \0
 */
char* get_next_token(char *pstr, unsigned int *size);

#endif
