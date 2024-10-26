#pragma once

#include "lowl_renderer.h"

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
    mg_pipeline pipeline;
    mg_render_pass render_pass;
    mg_framebuffer framebuffer;
    mg_image color_attachment;
    mg_image depth_stencil_attachment;
    
    mg_uniform_buffer global_ub; // bound while the layer is recording
    mg_uniform_buffer internal_ub; //bound while the layer is rendered

    mg_image_array image_array;
}
mg_render_layer;

typedef enum mg_world_flags_2d
{
    MG_WORLD_FLAG_2D_ENABLE_VIGNETTE = 0x1,
    MG_WORLD_FLAG_2D_ENABLE_LIGHTS = 0x2,
    MG_WORLD_FLAG_2D_ENABLE_SHADOWS = 0x4
}
mg_world_flags_2d;

typedef struct mg_world_info_2d
{
    uint64_t flags;
    
    mg_vec2 camera_position, camera_zoom;
    mg_vec2 shadow_offset;
    float shadow_opacity;
    mg_vec3 global_light_color;
    float global_light_intensity;
    //float brightness, grayscale;
}
mg_world_info_2d;

typedef struct mg_highl_renderer_init_info
{
    mg_platform *platform;
    mg_renderer_type type;
	uint32_t width, height;
}
mg_highl_renderer_init_info;

MG_API mg_render_layer *mg_highl_renderer_create_render_layer   (mg_shader_source vertex, mg_shader_source fragment,
    bool has_depth_stencil_attachment, size_t global_ub_size, size_t internal_ub_size);
MG_API void mg_highl_renderer_destroy_render_layer                (mg_render_layer *layer);
MG_API void mg_highl_renderer_resize_render_layer                 (mg_render_layer *layer, int32_t width, int32_t height);
MG_API void mg_highl_renderer_begin_render_layer                  (mg_render_layer *layer, mg_vec4 clear_value);
MG_API void mg_highl_renderer_end_render_layer                    (mg_render_layer *layer);

MG_API mg_render_layer *mg_highl_renderer_create_nonlit_layer_2d	(void);
MG_API mg_render_layer *mg_highl_renderer_create_world_layer_2d	(void);
MG_API mg_render_layer *mg_highl_renderer_create_world_layer_3d	(void);

MG_API void mg_highl_renderer_begin_background	  (mg_render_layer *layer, mg_vec4 clear_color);
MG_API void mg_highl_renderer_begin_world_2d      (mg_render_layer *layer, mg_world_info_2d *world_info);
MG_API void mg_highl_renderer_begin_nonlit_2d     (mg_render_layer *layer);

MG_API void mg_highl_renderer_initialize          (mg_highl_renderer_init_info *renderer_info);
MG_API void mg_highl_renderer_shutdown            (void);

MG_API void mg_highl_renderer_resize              (int32_t width, int32_t height);

MG_API void mg_highl_renderer_begin               (void);
MG_API void mg_highl_renderer_end                 (void);

MG_API void mg_highl_renderer_draw_rect_2d_internal     (mg_mat4 model, mg_vec4 color);
MG_API void mg_highl_renderer_draw_sprite_2d_internal   (mg_mat4 model, mg_vec4 color, mg_sprite *sprite);
MG_API void mg_highl_renderer_draw_text_2d_internal     (mg_mat4 model, mg_vec4 color, const char *text, mg_font *font);
MG_API void mg_highl_renderer_draw_mesh_2d_internal     (mg_mat4 model, mg_vec4 color, mg_mesh *mesh, mg_texture *texture);

MG_API void mg_highl_renderer_draw_rect_2d              (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color);
MG_API void mg_highl_renderer_draw_sprite_2d            (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, mg_sprite *sprite);
MG_API void mg_highl_renderer_draw_text_2d              (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, const char *text, mg_font *font);
MG_API void mg_highl_renderer_draw_mesh_2d              (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, mg_mesh *mesh, mg_texture *texture);

MG_API void mg_highl_renderer_draw_rotated_rect_2d		(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color);
MG_API void mg_highl_renderer_draw_rotated_sprite_2d	(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, mg_sprite *sprite);
MG_API void mg_highl_renderer_draw_rotated_text_2d		(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, const char *text, mg_font *font);
MG_API void mg_highl_renderer_draw_rotated_mesh_2d		(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, mg_mesh *mesh, mg_texture *texture);

MG_API void mg_highl_renderer_draw_line_2d          (mg_vec2 point1, mg_vec2 point2, float width, mg_vec4 color);

MG_API void mg_highl_renderer_draw_point_light_2d   (mg_vec2 position, float scale, float intensity, mg_vec3 color);

MG_API void mg_highl_renderer_begin_world_3d        (mg_render_layer *layer);
MG_API void mg_highl_renderer_end_world_3d          (mg_render_layer *layer);

MG_API void mg_highl_renderer_draw_mesh_3d          (mg_mat4 model, mg_mesh *mesh, mg_texture *texture);

#ifdef __cplusplus
}
#endif