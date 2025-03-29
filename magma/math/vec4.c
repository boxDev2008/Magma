#include "vec4.h"
#include "math.h"

#include <math.h>

mg_vec4 mg_vec4_new(float x, float y, float z, float w)
{
    mg_vec4 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec.w = w;
    return vec;
}

mg_vec4 mg_vec4_add(mg_vec4 a, mg_vec4 b)
{
    return mg_vec4_new(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

mg_vec4 mg_vec4_subtract(mg_vec4 a, mg_vec4 b)
{
    return mg_vec4_new(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

mg_vec4 mg_vec4_lerp(mg_vec4 a, mg_vec4 b, float by)
{
    return mg_vec4_new(mg_math_lerpf(a.x, b.x, by), mg_math_lerpf(a.y, b.y, by),
        mg_math_lerpf(a.z, b.z, by), mg_math_lerpf(a.w, b.w, by));
}

float mg_vec4_dot(mg_vec4 a, mg_vec4 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

float mg_vec4_magnitude(mg_vec4 vec)
{
    return sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z) + (vec.w * vec.w));
}

mg_vec4 mg_vec4_normalize(mg_vec4 vec)
{
    const float magnitude = mg_vec4_magnitude(vec);
    if (magnitude != 0)
    {
        const float inverse_magnitude = 1.0f / magnitude; 
        vec.x *= inverse_magnitude;
        vec.y *= inverse_magnitude;
        vec.z *= inverse_magnitude;
        vec.w *= inverse_magnitude;
    }
    return vec;
}

float mg_vec4_distance(mg_vec4 a, mg_vec4 b)
{
    mg_vec4 delta = mg_vec4_subtract(a, b);
    return mg_vec4_magnitude(delta);
}

bool mg_vec4_contains_vec2(mg_vec4 v4, mg_vec2 v2)
{
    return v4.x <= v2.x && v4.y <= v2.y && 
           v4.z >= v2.x && v4.w >= v2.y;
}

mg_vec4i mg_vec4i_new(int x, int y, int z, int w)
{
    mg_vec4i vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec.w = w;
    return vec;
}

mg_vec4i mg_vec4i_add(mg_vec4i a, mg_vec4i b)
{
    return mg_vec4i_new(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

mg_vec4i mg_vec4i_subtract(mg_vec4i a, mg_vec4i b)
{
    return mg_vec4i_new(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

int mg_vec4i_dot(mg_vec4i a, mg_vec4i b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

bool mg_vec4i_contains_vec2i(mg_vec4i v4, mg_vec2i v2)
{
    return v4.x <= v2.x && v4.y <= v2.y && 
           v4.z >= v2.x && v4.w >= v2.y;
}