#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MG_PI 3.14159265359f
#define MG_RAD2DEG 57.29578f
#define MG_DEG2RAD 0.01745329251f

MG_API double mg_math_clamp(double d, double min, double max);
MG_API float mg_math_clampf(float d, float min, float max);

MG_API double mg_math_clamp01(double d);
MG_API float mg_math_clamp01f(float d);

MG_API double mg_math_lerp(double first, double second, double by);
MG_API float mg_math_lerpf(float first, float second, float by);

#ifdef __cplusplus
}
#endif