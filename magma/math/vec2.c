#include "vec2.h"

#include <math.h>

mg_vec2_t mg_vec2(float x, float y)
{
    mg_vec2_t vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

mg_vec2_t mg_vec2_add(mg_vec2_t a, mg_vec2_t b)
{
    return mg_vec2(a.x + b.x, a.y + b.y);
}

mg_vec2_t mg_vec2_subtract(mg_vec2_t a, mg_vec2_t b)
{
    return mg_vec2(a.x - b.x, a.y - b.y);
}

float mg_vec2_dot(mg_vec2_t a, mg_vec2_t b)
{
    return (a.x * b.x) + (a.y * b.y);
}

float mg_vec2_magnitude(mg_vec2_t vec)
{
    return sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

mg_vec2_t mg_vec2_normalize(mg_vec2_t vec)
{
    float magnitude = mg_vec2_magnitude(vec);
    if (magnitude != 0)
    {
        vec.x /= magnitude;
        vec.y /= magnitude;
    }
    return vec;
}

mg_vec2i_t mg_vec2i(int x, int y)
{
    mg_vec2i_t vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

mg_vec2i_t mg_vec2i_add(mg_vec2i_t a, mg_vec2i_t b)
{
    return mg_vec2i(a.x + b.x, a.y + b.y);
}

mg_vec2i_t mg_vec2i_subtract(mg_vec2i_t a, mg_vec2i_t b)
{
    return mg_vec2i(a.x - b.x, a.y - b.y);
}

int mg_vec2i_dot(mg_vec2i_t a, mg_vec2i_t b)
{
    return (a.x * b.x) + (a.y * b.y);
}