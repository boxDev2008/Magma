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

float mg_vec4_distance(mg_vec4 a, mg_vec4 b)
{
    mg_vec4 delta = mg_vec4_subtract(a, b);
    return mg_vec4_magnitude(delta);
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