#include "vec3.h"
#include "math.h"

#include <math.h>

mg_vec3 mg_vec3_new(float x, float y, float z)
{
    mg_vec3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

mg_vec3 mg_vec3_add(mg_vec3 a, mg_vec3 b)
{
    return mg_vec3_new(a.x + b.x, a.y + b.y, a.z + b.z);
}

mg_vec3 mg_vec3_subtract(mg_vec3 a, mg_vec3 b)
{
    return mg_vec3_new(a.x - b.x, a.y - b.y, a.z - b.z);
}

mg_vec3 mg_vec3_multiply(mg_vec3 a, mg_vec3 b)
{
    return mg_vec3_new(a.x * b.x, a.y * b.y, a.z * b.z);
}

mg_vec3 mg_vec3_lerp(mg_vec3 a, mg_vec3 b, float by)
{
    return mg_vec3_new(mg_math_lerpf(a.x, b.x, by), mg_math_lerpf(a.y, b.y, by), mg_math_lerpf(a.z, b.z, by));
}

float mg_vec3_dot(mg_vec3 a, mg_vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

mg_vec3 mg_vec3_cross(mg_vec3 a, mg_vec3 b)
{
    mg_vec3 result;
    result.x = (a.y * b.z) - (a.z * b.y);
    result.y = (a.z * b.x) - (a.x * b.z);
    result.z = (a.x * b.y) - (a.y * b.x);
    return result;
}

float mg_vec3_magnitude(mg_vec3 vec)
{
    return sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

mg_vec3 mg_vec3_normalize(mg_vec3 vec)
{
    const float magnitude = mg_vec3_magnitude(vec);
    if (magnitude != 0)
    {
        const float inverse_magnitude = 1.0f / magnitude; 
        vec.x *= inverse_magnitude;
        vec.y *= inverse_magnitude;
        vec.z *= inverse_magnitude;
    }
    return vec;
}

float mg_vec3_distance(mg_vec3 a, mg_vec3 b)
{
    mg_vec3 delta = mg_vec3_subtract(a, b);
    return mg_vec3_magnitude(delta);
}

mg_vec3i mg_vec3i_new(int x, int y, int z)
{
    mg_vec3i vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

mg_vec3i mg_vec3i_add(mg_vec3i a, mg_vec3i b)
{
    return mg_vec3i_new(a.x + b.x, a.y + b.y, a.z + b.z);
}

mg_vec3i mg_vec3i_subtract(mg_vec3i a, mg_vec3i b)
{
    return mg_vec3i_new(a.x - b.x, a.y - b.y, a.z - b.z);
}

int mg_vec3i_dot(mg_vec3i a, mg_vec3i b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}