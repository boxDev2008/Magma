#pragma once

#include <magma/rendering/renderer.h>

#include "platform/platform.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/mat4.h"

#include "stb_truetype.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MG_HLGFX_FONT_TEXTURE_WIDTH
    #define MG_HLGFX_FONT_TEXTURE_WIDTH 1024
#endif

#ifndef MG_HLGFX_FONT_TEXTURE_HEIGHT
    #define MG_HLGFX_FONT_TEXTURE_HEIGHT 512
#endif

#ifndef MG_HLGFX_2D_MAX_GEOMETRY_COUNT
    #define MG_HLGFX_2D_MAX_GEOMETRY_COUNT (1 << 14)
#endif

typedef struct mg_camera_info_2d
{
	mg_vec2 position, zoom;
}
mg_camera_info_2d;

typedef struct mg_lit_scene_info_2d
{
    mg_vec3 global_light_color;
    float global_light_intensity;
}
mg_lit_scene_info_2d;

typedef enum mg_post_process_flag
{
    MG_POST_PROCESS_FLAG_VIGNETTE = 1 << 0
}
mg_post_process_flag;

typedef struct mg_post_process_info
{
    uint32_t flags;

    float vignette_strength;
    mg_vec3 vignette_color;
}
mg_post_process_info;

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
    mg_texture *texture;
    mg_vec4 texture_area;
    mg_vec2i size;
}
mg_sprite;

typedef struct mg_font
{
    stbtt_bakedchar cdata[96];
    mg_texture texture;
}
mg_font;

typedef struct mg_hlgfx_init_info
{
    mg_renderer_type type;
	uint32_t width, height;
}
mg_hlgfx_init_info;

void mg_hlgfx_initialize            (const mg_hlgfx_init_info *renderer_info);
void mg_hlgfx_shutdown              (void);

void mg_hlgfx_resize                (int32_t width, int32_t height);

void mg_hlgfx_begin                 (const mg_post_process_info *post_process_info, const mg_vec3 clear_color);
void mg_hlgfx_end                   (void);

void mg_hlgfx_begin_scene_2d			(const mg_camera_info_2d *camera_info);
void mg_hlgfx_end_scene_2d				(void);

void mg_hlgfx_begin_lit_scene_2d        (const mg_camera_info_2d *camera_info, mg_lit_scene_info_2d *lit_scene_info);
void mg_hlgfx_end_lit_scene_2d          (void);

void mg_hlgfx_draw_rect_2d_internal     (mg_mat4 model, mg_vec4 color);
void mg_hlgfx_draw_sprite_2d_internal   (mg_mat4 model, mg_vec4 color, const mg_sprite *sprite);
void mg_hlgfx_draw_sprite_2d_internal_ext(mg_mat4 model, mg_vec4 color, float grayscale, const mg_sprite *sprite);

void mg_hlgfx_draw_rect_2d              (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color);
void mg_hlgfx_draw_sprite_2d            (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, const mg_sprite *sprite);
void mg_hlgfx_draw_sprite_2d_ext        (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, float grayscale, const mg_sprite *sprite);

void mg_hlgfx_draw_text_2d              (mg_vec2 position, float scale, mg_vec4 color, mg_text_alignment alignment, mg_font *font, const char *fmt, ...);

void mg_hlgfx_draw_rotated_rect_2d		    (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color);
void mg_hlgfx_draw_rotated_sprite_2d	    (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, const mg_sprite *sprite);
void mg_hlgfx_draw_rotated_sprite_2d_ext    (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, float grayscale, const mg_sprite *sprite);

void mg_hlgfx_draw_point_light_2d       (mg_vec2 position, float scale, float intensity, mg_vec3 color);

mg_texture      mg_hlgfx_add_texture            (uint32_t width, uint32_t height, mg_sampler_filter filter, mg_pixel_format format, void *data);
mg_texture      mg_hlgfx_add_texture_from_file  (const char *file_name, mg_sampler_filter filter, mg_pixel_format format);
mg_font         mg_hlgfx_add_font               (void *data);
mg_font         mg_hlgfx_add_font_from_file     (const char* file_name);

#ifdef __cplusplus
}
#endif