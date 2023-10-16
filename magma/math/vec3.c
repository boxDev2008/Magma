#include "vec3.h"

#include <math.h>

mg_vec3_t mg_vec3(float x, float y, float z)
{
    mg_vec3_t vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

mg_vec3_t mg_vec3_add(mg_vec3_t a, mg_vec3_t b)
{
    return mg_vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

mg_vec3_t mg_vec3_subtract(mg_vec3_t a, mg_vec3_t b)
{
    return mg_vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

float mg_vec3_dot(mg_vec3_t a, mg_vec3_t b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

mg_vec3_t mg_vec3_cross(mg_vec3_t a, mg_vec3_t b)
{
    mg_vec3_t result;
    result.x = (a.y * b.z) - (a.z * b.y);
    result.y = (a.z * b.x) - (a.x * b.z);
    result.z = (a.x * b.y) - (a.y * b.x);
    return result;
}

float mg_vec3_magnitude(mg_vec3_t vec)
{
    return sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

mg_vec3_t mg_vec3_normalize(mg_vec3_t vec)
{
    float magnitude = mg_vec3_magnitude(vec);
    if (magnitude != 0)
    {
        vec.x /= magnitude;
        vec.y /= magnitude;
        vec.z /= magnitude;
    }
    return vec;
}

float mg_vec3_distance(mg_vec3_t a, mg_vec3_t b)
{
    mg_vec3_t delta = mg_vec3_subtract(a, b);
    return mg_vec3_magnitude(delta);
}

mg_vec3i_t mg_vec3i(int x, int y, int z)
{
    mg_vec3i_t vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

mg_vec3i_t mg_vec3i_add(mg_vec3i_t a, mg_vec3i_t b)
{
    return mg_vec3i(a.x + b.x, a.y + b.y, a.z + b.z);
}

mg_vec3i_t mg_vec3i_subtract(mg_vec3i_t a, mg_vec3i_t b)
{
    return mg_vec3i(a.x - b.x, a.y - b.y, a.z - b.z);
}

int mg_vec3i_dot(mg_vec3i_t a, mg_vec3i_t b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}