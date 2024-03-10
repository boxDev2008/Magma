#pragma once

#include "rendering/rhi_renderer.h"

#include "platform/platform.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/mat4.h"

#include "resources.h"

#ifndef MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS
    #define MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS 64
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_graphics_flags
{
    MG_GRAPHICS_FLAG_ENABLE_BACKGROUND = 0x1,
    MG_GRAPHICS_FLAG_ENABLE_WORLD_2D = 0x2,
    MG_GRAPHICS_FLAG_ENABLE_NONLIT_2D = 0x4,
    MG_GRAPHICS_FLAG_ENABLE_WORLD_3D = 0x8
}
mg_graphics_flags_t;

typedef enum mg_world_flags_2d
{
    MG_WORLD_FLAG_2D_ENABLE_VIGNETTE = 0x1,
    MG_WORLD_FLAG_2D_ENABLE_LIGHTS = 0x2,
    MG_WORLD_FLAG_2D_ENABLE_SHADOWS = 0x4
}
mg_world_flags_2d_t;

typedef struct mg_world_info_2d
{
    uint64_t flags;

    mg_vec2_t camera_position, camera_zoom;
    mg_vec2_t shadow_offset;
    float shadown_opacity;
    mg_vec3_t global_light_color;
    float global_light_intensity;
    //float brightness, grayscale;
}
mg_world_info_2d_t;

MG_API void mg_graphics_initialize          (mg_renderer_init_info_t *renderer_info, uint64_t flags);
MG_API void mg_graphics_shutdown            (void);

MG_API void mg_graphics_resize              (int32_t width, int32_t height);

MG_API void mg_graphics_begin               (void);
MG_API void mg_graphics_end                 (void);

MG_API void mg_graphics_2d_begin_world      (mg_world_info_2d_t *world_info);
MG_API void mg_graphics_2d_end_world        (void);

MG_API void mg_graphics_2d_begin_nonlit     (void);
MG_API void mg_graphics_2d_end_nonlit       (void);

MG_API void mg_graphics_2d_draw_rect        (mg_mat4_t model, mg_vec4_t color);
MG_API void mg_graphics_2d_draw_sprite      (mg_mat4_t model, mg_vec4_t color, mg_sprite_t *sprite);
MG_API void mg_graphics_2d_draw_line        (mg_vec2_t point1, mg_vec2_t point2, mg_vec4_t color);
MG_API void mg_graphics_2d_draw_text        (mg_mat4_t model, mg_vec4_t color, const char *text, mg_font_t *font);
MG_API void mg_graphics_2d_draw_point_light (mg_vec2_t position, float scale, float intensity, mg_vec3_t color);

MG_API void mg_graphics_3d_begin_world      (void);
MG_API void mg_graphics_3d_end_world        (void);

MG_API void mg_graphics_3d_draw_mesh        (mg_mat4_t model, mg_vec4_t color, mg_mesh_t *mesh, mg_texture_t *texture);

#ifdef __cplusplus
}
#endif