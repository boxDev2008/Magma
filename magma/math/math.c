#include "math.h"

double mg_math_clamp(double d, double min, double max)
{
    const double t = d < min ? min : d;
    return t > max ? max : t;
}

float mg_math_clampf(float d, float min, float max)
{
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

double mg_math_lerp(double first, double second, double by)
{
    return first * (1 - by) + second * by;
}

float mg_math_lerpf(float first, float second, float by)
{
    return first * (1 - by) + second * by;
}