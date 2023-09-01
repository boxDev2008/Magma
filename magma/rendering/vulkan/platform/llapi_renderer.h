#pragma once

#include "platform/platform.h"
#include "math/vec2.h"

typedef enum mg_renderer_type
{
    MG_RENDERER_TYPE_OPENGL,
    MG_RENDERER_TYPE_VULKAN
}
mg_renderer_type_t;

typedef enum mg_world_flags
{
    MG_WORLD_FLAG_BLOOM,
    MG_WORLD_FLAG_BLUR_SHADOWS,
    MG_WORLD_FLAG_BLUR_BACKGROUND
}
mg_world_flags_t;

typedef struct mg_world_info
{
    uint64_t flags;

    mg_vec2_t camera_position, camera_zoom;
    float shadown_opacity, shadow_offset;
    float brightness, grayscale;
}
mg_world_info_t;

typedef struct mg_texture
{
    int32_t width, height;
    void *internal_data;
}
mg_texture_t;

typedef enum mg_texture_filter
{
    MG_TEXTURE_FILTER_NEAREST,
    MG_TEXTURE_FILTER_LINEAR
}
mg_texture_filter_t;

typedef struct mg_texture_description
{
    int32_t width, height;
    uint8_t *pixels;
    mg_texture_filter_t filter;
}
mg_texture_description_t;

MG_API void mg_llapi_renderer_initialize    (mg_platform_t *platform, mg_renderer_type_t type);
MG_API void mg_llapi_renderer_shutdown      (void);

MG_API void mg_llapi_renderer_resize        (int32_t width, int32_t height);

MG_API void mg_llapi_renderer_begin_frame   (void);
MG_API void mg_llapi_renderer_end_frame     (void);

MG_API void mg_llapi_renderer_present_frame (void);