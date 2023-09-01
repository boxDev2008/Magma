#pragma once

#include "llapi_renderer.h"

#include "platform/platform.h"
#include "math/vec2.h"

typedef enum mg_world_flags mg_world_flags_t;
enum mg_world_flags
{
    MG_WORLD_FLAG_BLOOM,
    MG_WORLD_FLAG_BLUR_SHADOWS,
    MG_WORLD_FLAG_BLUR_BACKGROUND
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
    mg_descriptor_set_t descriptor_set;
};

typedef enum mg_texture_filter mg_texture_filter_t;
enum mg_texture_filter
{
    MG_TEXTURE_FILTER_NEAREST,
    MG_TEXTURE_FILTER_LINEAR
};

typedef struct mg_texture_description mg_texture_description_t;
struct mg_texture_description
{
    int32_t width, height;
    uint8_t *pixels;
    mg_texture_filter_t filter;
};