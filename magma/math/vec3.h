#pragma once

#include "base.h"

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
mg_vec3_t;

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
mg_vec3i_t;

MG_API mg_vec3_t    mg_vec3             (float x, float y, float z);
MG_API mg_vec3_t    mg_vec3_add         (mg_vec3_t a, mg_vec3_t b);
MG_API mg_vec3_t    mg_vec3_subtract    (mg_vec3_t a, mg_vec3_t b);
MG_API float        mg_vec3_dot         (mg_vec3_t a, mg_vec3_t b);
MG_API mg_vec3_t    mg_vec3_cross       (mg_vec3_t a, mg_vec3_t b);
MG_API float        mg_vec3_magnitude   (mg_vec3_t vec);
MG_API mg_vec3_t    mg_vec3_normalize   (mg_vec3_t vec);
MG_API float        mg_vec3_distance    (mg_vec3_t a, mg_vec3_t b);

MG_API mg_vec3i_t   mg_vec3i            (int x, int y, int z);
MG_API mg_vec3i_t   mg_vec3i_add        (mg_vec3i_t a, mg_vec3i_t b);
MG_API mg_vec3i_t   mg_vec3i_subtract   (mg_vec3i_t a, mg_vec3i_t b);
MG_API int          mg_vec3i_dot        (mg_vec3i_t a, mg_vec3i_t b);