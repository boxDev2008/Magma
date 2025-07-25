#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API float mg_randomf(void);
MG_API float mg_random_rangef(float min, float max);
MG_API int mg_random_rangei(int min, int max);

#ifdef __cplusplus
}
#endif