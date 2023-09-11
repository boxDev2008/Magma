#include "vec4.h"

#include <math.h>

mg_vec4_t mg_vec4(float x, float y, float z, float w)
{
    mg_vec4_t vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec.w = w;
    return vec;
}

mg_vec4_t mg_vec4_add(mg_vec4_t a, mg_vec4_t b)
{
    return mg_vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

mg_vec4_t mg_vec4_subtract(mg_vec4_t a, mg_vec4_t b)
{
    return mg_vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

float mg_vec4_dot(mg_vec4_t a, mg_vec4_t b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

float mg_vec4_magnitude(mg_vec4_t vec)
{
    return sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w));
}

mg_vec4_t mg_vec4_normalize(mg_vec4_t vec)
{
    float magnitude = mg_vec4_magnitude(vec);
    if (magnitude != 0)
    {
        vec.x /= magnitude;
        vec.y /= magnitude;
        vec.z /= magnitude;
        vec.w /= magnitude;
    }
    return vec;
}

mg_vec4i_t mg_vec4i(int x, int y, int z, int w)
{
    mg_vec4i_t vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec.w = w;
    return vec;
}

mg_vec4i_t mg_vec4i_add(mg_vec4i_t a, mg_vec4i_t b)
{
    return mg_vec4i(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

mg_vec4i_t mg_vec4i_subtract(mg_vec4i_t a, mg_vec4i_t b)
{
    return mg_vec4i(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

int mg_vec4i_dot(mg_vec4i_t a, mg_vec4i_t b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}