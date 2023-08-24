#pragma once

#include "defines.h"

typedef union vec2
{
    struct
    {
        f32 x;
        f32 y;
    };
    struct
    {
        f32 r;
        f32 g;
    };
    f32 v[2];
}
vec2;