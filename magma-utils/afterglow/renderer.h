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

#ifndef AG_FONT_TEXTURE_WIDTH
    #define AG_FONT_TEXTURE_WIDTH 1024
#endif

#ifndef AG_FONT_TEXTURE_HEIGHT
    #define AG_FONT_TEXTURE_HEIGHT 512
#endif

#ifndef AG_MAX_GEOMETRY_COUNT
    #define AG_MAX_GEOMETRY_COUNT (1 << 16)
#endif

typedef struct ag_camera_info
{
	mg_vec2 position, zoom;
}
ag_camera_info;

typedef enum ag_post_process_flag
{
    MG_POST_PROCESS_FLAG_VIGNETTE = 1 << 0,
    MG_POST_PROCESS_FLAG_GODRAYS = 1 << 1
}
ag_post_process_flag;

typedef struct ag_post_process_info
{
    uint32_t flags;

    mg_vec3 vignette_color;
    float vignette_strength;

    mg_vec2 godrays_offset;
    float godrays_density;

    mg_vec3 global_light_color;
    float global_light_intensity;
}
ag_post_process_info;

typedef enum ag_text_alignment
{
    MG_TEXT_ALIGNMENT_LEFT,
    MG_TEXT_ALIGNMENT_CENTER,
    MG_TEXT_ALIGNMENT_RIGHT
}
ag_text_alignment;

typedef struct ag_texture
{
    uint32_t width, height;
    uint8_t id;
}
ag_texture;

typedef struct ag_sprite
{
    ag_texture *texture;
    mg_vec4 texture_area;
    int32_t width, height;
}
ag_sprite;

typedef struct ag_font
{
    stbtt_bakedchar cdata[96];
    ag_texture texture;
}
ag_font;

typedef struct ag_init_info
{
    mg_renderer_type type;
	uint32_t width, height;
}
ag_init_info;

void ag_initialize            (const ag_init_info *renderer_info);
void ag_shutdown              (void);

void ag_resize                (int32_t width, int32_t height);

void ag_begin                 (void);
void ag_end                   (void);

void ag_bind_world            (const ag_post_process_info *post_process_info);
void ag_bind_background       (mg_vec3 clear_color);
void ag_bind_ui               (void);

void ag_begin_scene			(const ag_camera_info *camera_info);
void ag_end_scene				(void);

void ag_draw_rect_internal     (mg_mat4 model, mg_vec4 color);
void ag_draw_sprite_internal   (mg_mat4 model, mg_vec4 color, const ag_sprite *sprite);
void ag_draw_sprite_internal_ext(mg_mat4 model, mg_vec4 color, float grayscale, const ag_sprite *sprite);

void ag_draw_rect              (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color);
void ag_draw_sprite            (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, const ag_sprite *sprite);
void ag_draw_sprite_ext        (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, float grayscale, const ag_sprite *sprite);

void ag_draw_text              (mg_vec2 position, float scale, mg_vec4 color, ag_text_alignment alignment, ag_font *font, const char *fmt, ...);

void ag_draw_rotated_rect		    (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color);
void ag_draw_rotated_sprite	    (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, const ag_sprite *sprite);
void ag_draw_rotated_sprite_ext    (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, float grayscale, const ag_sprite *sprite);

void ag_draw_point_light       (mg_vec2 position, float scale, float intensity, mg_vec3 color);
void ag_draw_reflective_rect   (mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec3 color, float wave_intensity, float blur_amount);

ag_texture      ag_add_texture            (uint32_t width, uint32_t height, mg_sampler_filter filter, mg_pixel_format format, void *data);
ag_texture      ag_add_texture_from_file  (const char *file_name, mg_sampler_filter filter, mg_pixel_format format);
ag_font         ag_add_font               (void *data);
ag_font         ag_add_font_from_file     (const char* file_name);

#ifdef __cplusplus
}
#endif