#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "typedefs.h"
#include "memory.h"

typedef struct vector3 {
    float x;
    float y;
    float z;
} vector3;

typedef struct ivector3 {
    s32 x;
    s32 y;
    s32 z;
} ivector3; 

typedef struct matrix3 {
    vector3 x;
    vector3 y;
    vector3 z;
} matrix3;

extern matrix3 projection;

void init_vector3(vector3 *, float x, float y, float z);

#define DOT_PRODUCT3(v1, v2) v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

void m3_mul_v3(matrix3 *, vector3 *, vector3 *);

#endif 
