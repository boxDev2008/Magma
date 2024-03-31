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

#ifndef MG_CONFIG_GRAPHICS_MAX_RENDER_LAYERS
    #define MG_CONFIG_GRAPHICS_MAX_RENDER_LAYERS 16
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_render_layer
{
    mg_pipeline_t pipeline;
    mg_render_pass_t render_pass;
    mg_framebuffer_t framebuffer;
    mg_image_t color_attachment;
    mg_image_t depth_stencil_attachment;

    mg_uniform_buffer_t global_ub; // bound while the layer is recording
    mg_uniform_buffer_t internal_ub; //bound while the layer is rendered
}
mg_render_layer_t;

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

MG_API mg_render_layer_t *mg_graphics_create_render_layer   (mg_shader_source_t vertex_shader, mg_shader_source_t fragment_shader,
bool has_depth_stencil_attachment, size_t global_ub_size, size_t internal_ub_size);
MG_API void mg_graphics_destroy_render_layer                (mg_render_layer_t *layer);
MG_API void mg_graphics_add_render_layer                    (mg_render_layer_t *layer);
MG_API void mg_graphics_begin_render_layer                  (mg_render_layer_t *layer, mg_vec4_t clear_value);
MG_API void mg_graphics_end_render_layer                    (mg_render_layer_t *layer);

MG_API mg_render_layer_t *mg_graphics_create_nonlit_layer_2d    (void);
MG_API mg_render_layer_t *mg_graphics_create_world_layer_2d     (void);
MG_API mg_render_layer_t *mg_graphics_create_world_layer_3d     (void);

MG_API void mg_graphics_initialize          (mg_renderer_init_info_t *renderer_info);
MG_API void mg_graphics_shutdown            (void);

MG_API void mg_graphics_resize              (int32_t width, int32_t height);

MG_API void mg_graphics_begin               (void);
MG_API void mg_graphics_end                 (void);

MG_API void mg_graphics_2d_begin_background (mg_render_layer_t *layer, mg_vec4_t clear_color);
MG_API void mg_graphics_2d_end_background   (mg_render_layer_t *layer);

MG_API void mg_graphics_2d_begin_world      (mg_render_layer_t *layer, mg_world_info_2d_t *world_info);
MG_API void mg_graphics_2d_end_world        (mg_render_layer_t *layer);

MG_API void mg_graphics_2d_begin_nonlit     (mg_render_layer_t *layer);
MG_API void mg_graphics_2d_end_nonlit       (mg_render_layer_t *layer);

MG_API void mg_graphics_2d_draw_rect        (mg_mat4_t model, mg_vec4_t color);
MG_API void mg_graphics_2d_draw_sprite      (mg_mat4_t model, mg_vec4_t color, mg_sprite_t *sprite);
MG_API void mg_graphics_2d_draw_line        (mg_vec2_t point1, mg_vec2_t point2, float width, mg_vec4_t color);
MG_API void mg_graphics_2d_draw_text        (mg_mat4_t model, mg_vec4_t color, const char *text, mg_font_t *font);

MG_API void mg_graphics_2d_draw_point_light (mg_vec2_t position, float scale, float intensity, mg_vec3_t color);

MG_API void mg_graphics_3d_begin_world      (mg_render_layer_t *layer);
MG_API void mg_graphics_3d_end_world        (mg_render_layer_t *layer);

MG_API void mg_graphics_3d_draw_mesh        (mg_mat4_t model, mg_vec4_t color, mg_mesh_t *mesh, mg_texture_t *texture);

#ifdef __cplusplus
}
#endif