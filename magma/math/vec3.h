#pragma once

#include "defines.h"

typedef union vec3
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };
    struct
    {
        f32 r;
        f32 g;
        f32 b;
    };
    f32 v[3];
}
vec3;