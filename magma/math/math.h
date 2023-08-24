#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#define MG_PI 3.14159265359f
#define MG_RAD2DEG 57.29578f

MG_API f64 mg_math_clamp(f64 d, f64 min, f64 max);
MG_API f64 mg_math_lerp(f64 first, f64 second, f64 by);