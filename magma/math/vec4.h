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