#include "matrix.h"

matrix3 projection = {
    {
        1, 0, 0
    },
    {
        0, 1, 0
    },
    {
        0, 0, 1
    }
};

void init_vector3(vector3 *v3, float x, float y, float z) {
    v3->x = x;
    v3->y = y;
    v3->z = z;
}

void m3_mul_v3(matrix3 *m3, vector3 *v3, vector3 *result) {
    result->x = DOT_PRODUCT3(m3->x, (*v3));
    result->y = DOT_PRODUCT3(m3->y, (*v3));
    result->z = DOT_PRODUCT3(m3->z, (*v3));
}
