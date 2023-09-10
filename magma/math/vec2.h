#pragma once

#include "base.h"

typedef union mg_vec2
{
    struct
    {
        float x;
        float y;
    };
    struct
    {
        float r;
        float g;
    };
    float v[2];
}
mg_vec2_t;

typedef union mg_vec2i
{
    struct
    {
        int x;
        int y;
    };
    struct
    {
        int r;
        int g;
    };
    int v[2];
}
mg_vec2i_t;

MG_API mg_vec2_t    mg_vec2             (float x, float y);
MG_API mg_vec2_t    mg_vec2_add         (mg_vec2_t a, mg_vec2_t b);
MG_API mg_vec2_t    mg_vec2_subtract    (mg_vec2_t a, mg_vec2_t b);
MG_API float        mg_vec2_dot         (mg_vec2_t a, mg_vec2_t b);
MG_API float        mg_vec2_magnitude   (mg_vec2_t vec);
MG_API mg_vec2_t    mg_vec2_normalize   (mg_vec2_t vec);

MG_API mg_vec2i_t   mg_vec2i            (int x, int y);
MG_API mg_vec2i_t   mg_vec2i_add        (mg_vec2i_t a, mg_vec2i_t b);
MG_API mg_vec2i_t   mg_vec2i_subtract   (mg_vec2i_t a, mg_vec2i_t b);
MG_API int          mg_vec2i_dot        (mg_vec2i_t a, mg_vec2i_t b);