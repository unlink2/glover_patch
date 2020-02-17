#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

typedef unsigned long DWORD_T;
#ifdef __LP64__
typedef unsigned long WORD_T;
#else
typedef unsigned int WORD_T;
#endif
typedef unsigned short HWORD_T;
typedef unsigned char BYTE_T;

typedef char s8;
typedef short s16;
typedef int s32;

typedef BYTE_T u8;
typedef HWORD_T u16;
typedef WORD_T u32;

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif
