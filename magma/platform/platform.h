#pragma once

#include "base.h"

#include "math/vec2.h"

typedef struct mg_platform
{
    void *handle;
}
mg_platform_t;

typedef struct mg_platform_init_info
{
    const char *title;
    int32_t position_x, position_y;
    int32_t width, height;
}
mg_platform_init_info_t;

MG_API mg_platform_t   *mg_platform_initialize      (mg_platform_init_info_t *create_info);
MG_API void             mg_platform_shutdown        (mg_platform_t *platform);

MG_API void             mg_platform_poll_messages   (mg_platform_t *platform);
MG_API double           mg_platform_get_time        (void);

MG_API void             mg_platform_get_window_size (mg_platform_t *platform, int32_t *width, int32_t *height);