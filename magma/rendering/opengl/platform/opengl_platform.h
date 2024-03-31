#pragma once

#include "platform/platform.h"

void mg_opengl_platform_initialize  (mg_platform_t *platform);
void mg_opengl_platform_shutdown    (void);
void mg_opengl_platform_swapbuffers (void);