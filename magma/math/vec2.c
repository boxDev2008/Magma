#include "vec2.h"
#include "math.h"

#include <math.h>

mg_vec2 mg_vec2_new(float x, float y)
{
    mg_vec2 vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

mg_vec2 mg_vec2_add(mg_vec2 a, mg_vec2 b)
{
    return mg_vec2_new(a.x + b.x, a.y + b.y);
}

mg_vec2 mg_vec2_subtract(mg_vec2 a, mg_vec2 b)
{
    return mg_vec2_new(a.x - b.x, a.y - b.y);
}

mg_vec2 mg_vec2_multiply(mg_vec2 a, mg_vec2 b)
{
    return mg_vec2_new(a.x * b.x, a.y * b.y);
}

mg_vec2 mg_vec2_lerp(mg_vec2 a, mg_vec2 b, float by)
{
    return mg_vec2_new(mg_math_lerpf(a.x, b.x, by), mg_math_lerpf(a.y, b.y, by));
}

float mg_vec2_dot(mg_vec2 a, mg_vec2 b)
{
    return (a.x * b.x) + (a.y * b.y);
}

float mg_vec2_magnitude(mg_vec2 vec)
{
    return sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

mg_vec2 mg_vec2_normalize(mg_vec2 vec)
{
    const float magnitude = mg_vec2_magnitude(vec);
    if (magnitude != 0)
    {
        const float inverse_magnitude = 1.0f / magnitude; 
        vec.x *= inverse_magnitude;
        vec.y *= inverse_magnitude;
    }
    return vec;
}

float mg_vec2_distance(mg_vec2 a, mg_vec2 b)
{
    mg_vec2 delta = mg_vec2_subtract(a, b);
    return mg_vec2_magnitude(delta);
}

mg_vec2 mg_vec2_from_angle(float angle)
{
    float x = cosf(angle);
    float y = sinf(angle);
    return mg_vec2_new(x - y, y + x);
}

mg_vec2i mg_vec2i_new(int x, int y)
{
    mg_vec2i vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

mg_vec2i mg_vec2i_add(mg_vec2i a, mg_vec2i b)
{
    return mg_vec2i_new(a.x + b.x, a.y + b.y);
}

mg_vec2i mg_vec2i_subtract(mg_vec2i a, mg_vec2i b)
{
    return mg_vec2i_new(a.x - b.x, a.y - b.y);
}

int mg_vec2i_dot(mg_vec2i a, mg_vec2i b)
{
    return (a.x * b.x) + (a.y * b.y);
}