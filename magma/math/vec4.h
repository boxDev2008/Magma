#pragma once

#include "base.h"

typedef union mg_vec4
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    struct
    {
        float r;
        float g;
        float b;
        float a;
    };
    float v[4];
}
mg_vec4_t;

typedef union mg_vec4i
{
    struct
    {
        int x;
        int y;
        int z;
        int w;
    };
    struct
    {
        int r;
        int g;
        int b;
        int a;
    };
    int v[4];
}
mg_vec4i_t;

MG_API mg_vec4_t    mg_vec4             (float x, float y, float z, float w);
MG_API mg_vec4_t    mg_vec4_add         (mg_vec4_t a, mg_vec4_t b);
MG_API mg_vec4_t    mg_vec4_subtract    (mg_vec4_t a, mg_vec4_t b);
MG_API float        mg_vec4_dot         (mg_vec4_t a, mg_vec4_t b);
MG_API float        mg_vec4_magnitude   (mg_vec4_t vec);
MG_API mg_vec4_t    mg_vec4_normalize   (mg_vec4_t vec);

MG_API mg_vec4i_t   mg_vec4i            (int x, int y, int z, int w);
MG_API mg_vec4i_t   mg_vec4i_add        (mg_vec4i_t a, mg_vec4i_t b);
MG_API mg_vec4i_t   mg_vec4i_subtract   (mg_vec4i_t a, mg_vec4i_t b);
MG_API int          mg_vec4i_dot        (mg_vec4i_t a, mg_vec4i_t b);