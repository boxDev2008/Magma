#include "math.h"

f64 mg_math_clamp(f64 d, f64 min, f64 max)
{
    const double t = d < min ? min : d;
    return t > max ? max : t;
}

f64 mg_math_lerp(f64 first, f64 second, f64 by)
{
    return first * (1 - by) + second * by;
}