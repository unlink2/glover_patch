#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#ifdef __LP64__
typedef unsigned long WORD_T;
#else
typedef unsigned int WORD_T;
#endif
typedef unsigned short HWORD_T;
typedef unsigned char BYTE_T;

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif
