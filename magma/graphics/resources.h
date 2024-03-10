#pragma once

#include "rendering/rhi_renderer.h"

#include <stb_truetype.h>

#ifndef MG_CONFIG_GRAPHICS_MAX_TEXT_LENGTH
    #define MG_CONFIG_GRAPHICS_MAX_TEXT_LENGTH 256
#endif

#ifndef MG_CONFIG_GRAPHICS_FONT_HEIGHT
    #define MG_CONFIG_GRAPHICS_FONT_HEIGHT 86.0f
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_texture
{
    uint32_t width, height;
    mg_image_t image;
    mg_sampler_t sampler;
}
mg_texture_t;

typedef struct mg_sprite
{
    float x, y;
    float width, height;
    mg_texture_t *texture;
}
mg_sprite_t;

typedef struct mg_font
{
    stbtt_bakedchar cdata[96];
    uint8_t bitmap[1024 * 1024];
    mg_texture_t *texture;
}
mg_font_t;

typedef struct mg_mesh
{
    mg_vertex_buffer_t vertex_buffer;
    mg_index_buffer_t index_buffer;
    uint32_t index_count;
}
mg_mesh_t;

typedef struct mg_mesh_vertex
{
    mg_vec3_t position;
    mg_vec3_t normal;
    mg_vec2_t texture_coord;
}
mg_mesh_vertex_t;

MG_API mg_texture_t    *mg_graphics_create_texture              (uint32_t width, uint32_t height, void *data);
MG_API mg_texture_t    *mg_graphics_create_texture_from_file    (const char *file_name);

MG_API void             mg_graphics_destroy_texture             (mg_texture_t *texture);

MG_API mg_font_t       *mg_graphics_create_font                 (void *ttf_data);
MG_API mg_font_t       *mg_graphics_create_font_from_file       (const char *file_name);

MG_API void             mg_graphics_destroy_font                (mg_font_t *font);

MG_API mg_mesh_t       *mg_graphics_create_mesh                 (mg_mesh_vertex_t *vertices, uint32_t vertices_count, uint32_t *indices, uint32_t index_count);
MG_API void             mg_graphics_destroy_mesh                (mg_mesh_t *mesh);

#ifdef __cplusplus
}
#endif