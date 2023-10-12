#pragma once

#include "rendering/rhi_renderer.h"

#include "platform/platform.h"
#include "math/vec2.h"

typedef enum mg_world_flags mg_world_flags_t;
enum mg_world_flags
{
    MG_WORLD_FLAG_BLOOM = 1 << 0,
    MG_WORLD_FLAG_GLOW = 1 << 1,
    MG_WORLD_FLAG_BLUR_SHADOWS = 1 << 2,
    MG_WORLD_FLAG_VIGNETTE = 1 << 3
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
};

typedef struct mg_sprite mg_sprite_t;
struct mg_sprite
{
    int32_t x, y;
    int32_t width, height;
    mg_texture_t *texture;
};

MG_API void             mg_graphics_2d_initialize           (int32_t width, int32_t height);
MG_API void             mg_graphics_2d_shutdown             (void);
MG_API void             mg_graphics_2d_resize               (int32_t width, int32_t height);

MG_API void             mg_graphics_2d_begin                (void);
MG_API void             mg_graphics_2d_end                  (void);

MG_API void             mg_graphics_2d_begin_world          (mg_world_info_t *world_info);
MG_API void             mg_graphics_2d_end_world            (void);

MG_API void             mg_graphics_2d_draw_rect            (mg_vec2_t position, mg_vec2_t scale, mg_vec4_t color);
MG_API void             mg_graphics_2d_draw_rotated_rect    (mg_vec2_t position, mg_vec2_t scale, float rotation, mg_vec4_t color);

MG_API void             mg_graphics_2d_draw_sprite          (mg_vec2_t position, mg_vec2_t scale, mg_vec4_t color, mg_sprite_t *sprite);
MG_API void             mg_graphics_2d_draw_rotated_sprite  (mg_vec2_t position, mg_vec2_t scale, float rotation, mg_vec4_t color, mg_sprite_t *sprite);

MG_API mg_texture_t    *mg_graphics_2d_create_texture           (uint32_t width, uint32_t height, void *data);
MG_API mg_texture_t    *mg_graphics_2d_create_texture_from_file (const char *file_name);
MG_API void             mg_graphics_2d_destroy_texture          (mg_texture_t *texture);

MG_API mg_sprite_t     *mg_graphics_2d_create_sprite            (uint32_t x, uint32_t y, uint32_t width, uint32_t height, mg_texture_t *texture);
MG_API void             mg_graphics_2d_destroy_sprite           (mg_sprite_t *sprite);