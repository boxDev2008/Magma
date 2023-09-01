#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#define MG_PI 3.14159265359f
#define MG_RAD2DEG 57.29578f

MG_API double mg_math_clamp(double d, double min, double max);
MG_API double mg_math_lerp(double first, double second, double by);