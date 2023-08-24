#pragma once

#include "defines.h"

#include "math/vec2.h"

typedef struct mg_platform
{
    void *handle;
}
mg_platform_t;

typedef struct mg_window_description
{
    const char *title;
    i32 position_x, position_y;
    i32 width, height;
}
mg_window_description_t;

MG_API b8   mg_platform_initialize      (mg_platform_t *platform, mg_window_description_t *win_description);
MG_API void mg_platform_shutdown        (mg_platform_t *platform);

MG_API b8   mg_platform_poll_messages   (mg_platform_t *platform);
MG_API f64  mg_platform_get_time        (void);

MG_API void mg_platform_get_window_size (mg_platform_t *platform, i32 *width, i32 *height);