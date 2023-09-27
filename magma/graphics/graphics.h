#pragma once

#include "rhi_renderer.h"

#include "platform/platform.h"
#include "math/vec2.h"

typedef enum mg_world_flags mg_world_flags_t;
enum mg_world_flags
{
    MG_WORLD_FLAG_BLOOM,
    MG_WORLD_FLAG_GLOW,
    MG_WORLD_FLAG_BLUR_SHADOWS,
};

typedef struct mg_world_info mg_world_info_t;
struct mg_world_info
{
    uint64_t flags;

    mg_vec2_t camera_position, camera_zoom;
    float shadown_opacity, shadow_offset;
    float brightness, grayscale;
};

typedef struct mg_texture mg_texture_t;
struct mg_texture
{
    int32_t width, height;
    mg_image_t image;
    mg_sampler_t sampler;
    mg_descriptor_set_t descriptor_set;
};

MG_API void mg_graphics_initialize          (void);
MG_API void mg_graphics_shutdown            (void);
MG_API void mg_graphics_resize              (uint32_t width, uint32_t height);

MG_API void mg_graphics_begin_background    (void);
MG_API void mg_graphics_end_background      (void);

MG_API void mg_graphics_begin_world         (mg_world_info_t *world_info);
MG_API void mg_graphics_end_world           (void);