#pragma once

#include <magma/rendering/renderer.h>

#include "platform/platform.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/mat4.h"

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

    size_t internal_ub_size;
    void *internal_ub_data;

    mg_image_array image_array;
}
mg_render_layer;

typedef enum mg_world_flags_2d
{
    MG_WORLD_FLAG_2D_ENABLE_VIGNETTE = 0x1,
    MG_WORLD_FLAG_2D_ENABLE_LIGHTS = 0x2,
    //MG_WORLD_FLAG_2D_ENABLE_SHADOWS = 0x4
}
mg_world_flags_2d;

typedef struct mg_world_info_2d
{
    uint64_t flags;
    
    mg_vec2 camera_position, camera_zoom;
    //mg_vec2 shadow_offset;
    //float shadow_opacity;
    mg_vec3 global_light_color;
    float global_light_intensity;
    //float brightness, grayscale;
}
mg_world_info_2d;

typedef enum mg_text_alignment
{
    MG_TEXT_ALIGNMENT_LEFT,
    MG_TEXT_ALIGNMENT_CENTER,
    MG_TEXT_ALIGNMENT_RIGHT
}
mg_text_alignment;

typedef struct mg_texture
{
    uint32_t width, height;
    uint8_t id;
}
mg_texture;

typedef struct mg_sprite
{
    float x, y;
    float width, height;
    mg_texture *texture;
}
mg_sprite;

typedef struct mg_hlgfx_init_info
{
    mg_platform *platform;
    mg_renderer_type type;
	uint32_t width, height;
}
mg_hlgfx_init_info;

mg_render_layer *mg_hlgfx_create_render_layer   (mg_shader shader,
    bool has_depth_stencil_attachment, size_t global_ub_size, size_t internal_ub_size);
void mg_hlgfx_destroy_render_layer                (mg_render_layer *layer);
void mg_hlgfx_resize_render_layer                 (mg_render_layer *layer, int32_t width, int32_t height);
void mg_hlgfx_begin_render_layer                  (mg_render_layer *layer, mg_vec4 clear_value);
void mg_hlgfx_end_render_layer                    (void);

mg_render_layer *mg_hlgfx_create_raw_layer_2d	(void);
mg_render_layer *mg_hlgfx_create_world_layer_2d	(void);
mg_render_layer *mg_hlgfx_create_world_layer_3d	(void);

void mg_hlgfx_begin_world_2d        (mg_render_layer *layer, mg_vec4 clear_color, mg_world_info_2d *world_info);
void mg_hlgfx_begin_world_3d        (mg_render_layer *layer, mg_vec4 clear_color);
void mg_hlgfx_begin_raw_2d          (mg_render_layer *layer, mg_vec4 clear_color);

void mg_hlgfx_end_world_2d          (mg_render_layer *layer);
void mg_hlgfx_end_world_3d          (void);
void mg_hlgfx_end_raw_2d            (void);

void mg_hlgfx_initialize            (mg_hlgfx_init_info *renderer_info);
void mg_hlgfx_shutdown              (void);

void mg_hlgfx_resize                (int32_t width, int32_t height);

void mg_hlgfx_begin                 (void);
void mg_hlgfx_end                   (void);

void mg_hlgfx_draw_rect_2d_internal     (mg_mat4 model, mg_vec4 color);
void mg_hlgfx_draw_sprite_2d_internal   (mg_mat4 model, mg_vec4 color, mg_sprite *sprite);
//void mg_hlgfx_draw_text_2d_internal     (mg_mat4 model, mg_vec4 color, mg_text_alignment alignment, const char *text, mg_font *font);

void mg_hlgfx_draw_rect_2d              (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color);
void mg_hlgfx_draw_sprite_2d            (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, mg_sprite *sprite);
//void mg_hlgfx_draw_text_2d              (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, mg_text_alignment alignment, const char *text, mg_font *font);

void mg_hlgfx_draw_rotated_rect_2d		    (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color);
void mg_hlgfx_draw_rotated_sprite_2d	    (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, mg_sprite *sprite);

//void mg_hlgfx_draw_rotated_text_2d		(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, mg_text_alignment alignment, const char *text, mg_font *font);

void mg_hlgfx_draw_line_2d          (mg_vec2 point1, mg_vec2 point2, float width, mg_vec4 color);

void mg_hlgfx_draw_point_light_2d   (mg_vec2 position, float scale, float intensity, mg_vec3 color);

//void mg_hlgfx_draw_mesh             (mg_mat4 model, mg_mesh *mesh, mg_texture *texture);

mg_texture   mg_hlgfx_add_texture               (uint32_t width, uint32_t height, mg_sampler_filter filter, mg_pixel_format format, void *data);
mg_texture   mg_hlgfx_add_texture_from_file     (const char *file_name, mg_sampler_filter filter, mg_pixel_format format);

void         mg_hlgfx_build_textures            (void);
void         mg_hlgfx_destroy_textures          (void);

void         mg_hlgfx_submit_sprite_batch       (void);

#ifdef __cplusplus
}
#endif