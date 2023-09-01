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