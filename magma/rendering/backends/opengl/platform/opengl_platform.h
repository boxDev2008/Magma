#pragma once

#include "platform/platform.h"

void mg_opengl_platform_initialize  (void);
void mg_opengl_platform_shutdown    (void);
void mg_opengl_platform_swapbuffers (void);
void mg_opengl_platform_set_vsync   (bool enabled);