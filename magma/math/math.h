#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MG_PI 3.14159265359f
#define MG_RAD2DEG 57.29578f
#define MG_DEG2RAD 0.01745329251f

MG_API inline double mg_math_clamp(double d, double min, double max);
MG_API inline float mg_math_clampf(float d, float min, float max);
MG_API inline int mg_math_clampi(int d, int min, int max);

MG_API inline double mg_math_clamp01(double d);
MG_API inline float mg_math_clamp01f(float d);
MG_API inline int mg_math_clamp01i(int d);

MG_API inline double mg_math_lerp(double first, double second, double by);
MG_API inline float mg_math_lerpf(float first, float second, float by);

#ifdef __cplusplus
}
#endif