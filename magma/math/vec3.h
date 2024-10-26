#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union mg_vec3
{
    struct
    {
        float x;
        float y;
        float z;
    };
    struct
    {
        float r;
        float g;
        float b;
    };
    float v[3];
}
mg_vec3;

typedef union mg_vec3i
{
    struct
    {
        int x;
        int y;
        int z;
    };
    struct
    {
        int r;
        int g;
        int b;
    };
    int v[3];
}
mg_vec3i;

MG_API mg_vec3  mg_vec3_new         (float x, float y, float z);
MG_API mg_vec3  mg_vec3_add         (mg_vec3 a, mg_vec3 b);
MG_API mg_vec3  mg_vec3_subtract    (mg_vec3 a, mg_vec3 b);
MG_API mg_vec3  mg_vec3_lerp        (mg_vec3 a, mg_vec3 b, float by);

MG_API float    mg_vec3_dot         (mg_vec3 a, mg_vec3 b);
MG_API mg_vec3  mg_vec3_cross       (mg_vec3 a, mg_vec3 b);
MG_API float    mg_vec3_magnitude   (mg_vec3 vec);
MG_API mg_vec3  mg_vec3_normalize   (mg_vec3 vec);
MG_API float    mg_vec3_distance    (mg_vec3 a, mg_vec3 b);

MG_API mg_vec3i mg_vec3i_new        (int x, int y, int z);
MG_API mg_vec3i mg_vec3i_add        (mg_vec3i a, mg_vec3i b);
MG_API mg_vec3i mg_vec3i_subtract   (mg_vec3i a, mg_vec3i b);
MG_API int      mg_vec3i_dot        (mg_vec3i a, mg_vec3i b);

#ifdef __cplusplus
}
#endif