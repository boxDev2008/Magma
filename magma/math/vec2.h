#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

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
mg_vec2;

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
mg_vec2i;

MG_API mg_vec2  mg_vec2_new         (float x, float y);
MG_API mg_vec2  mg_vec2_add         (mg_vec2 a, mg_vec2 b);
MG_API mg_vec2  mg_vec2_subtract    (mg_vec2 a, mg_vec2 b);
MG_API mg_vec2  mg_vec2_multiply    (mg_vec2 a, mg_vec2 b);
MG_API mg_vec2  mg_vec2_scale       (mg_vec2 a, float scale);
MG_API mg_vec2  mg_vec2_lerp        (mg_vec2 a, mg_vec2 b, float by);

MG_API float    mg_vec2_dot         (mg_vec2 a, mg_vec2 b);
MG_API float    mg_vec2_magnitude   (mg_vec2 vec);
MG_API mg_vec2  mg_vec2_normalize   (mg_vec2 vec);
MG_API float    mg_vec2_distance    (mg_vec2 a, mg_vec2 b);
MG_API mg_vec2  mg_vec2_from_angle  (float angle);

MG_API mg_vec2i mg_vec2i_new        (int x, int y);
MG_API mg_vec2i mg_vec2i_add        (mg_vec2i a, mg_vec2i b);
MG_API mg_vec2i mg_vec2i_subtract   (mg_vec2i a, mg_vec2i b);
MG_API int      mg_vec2i_dot        (mg_vec2i a, mg_vec2i b);

#ifdef __cplusplus
}
#endif