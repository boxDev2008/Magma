#pragma once

#include "base.h"
#include "vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

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
mg_vec4;

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
mg_vec4i;

MG_API mg_vec4 mg_vec4_new (float x, float y, float z, float w);

MG_API mg_vec4  mg_vec4_add         (mg_vec4 a, mg_vec4 b);
MG_API mg_vec4  mg_vec4_subtract    (mg_vec4 a, mg_vec4 b);
MG_API mg_vec4  mg_vec4_lerp        (mg_vec4 a, mg_vec4 b, float by);

MG_API float    mg_vec4_dot         (mg_vec4 a, mg_vec4 b);
MG_API float    mg_vec4_magnitude   (mg_vec4 vec);
MG_API mg_vec4  mg_vec4_normalize   (mg_vec4 vec);
MG_API float    mg_vec4_distance    (mg_vec4 a, mg_vec4 b);
MG_API bool     mg_vec4_contains_vec2   (mg_vec4 vec4, mg_vec2 vec2);

MG_API mg_vec4i mg_vec4i_new        (int x, int y, int z, int w);
MG_API mg_vec4i mg_vec4i_add        (mg_vec4i a, mg_vec4i b);
MG_API mg_vec4i mg_vec4i_subtract   (mg_vec4i a, mg_vec4i b);
MG_API int      mg_vec4i_dot        (mg_vec4i a, mg_vec4i b);

MG_API bool     mg_vec4i_contains_vec2i (mg_vec4i vec4, mg_vec2i vec2);

#ifdef __cplusplus
}
#endif