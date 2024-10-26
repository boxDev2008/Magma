#pragma once

#include "rendering/lowl_renderer.h"

#include <stb_truetype.h>

#ifndef MG_CONFIG_HIGHL_RENDERER_MAX_TEXT_LENGTH
    #define MG_CONFIG_HIGHL_RENDERER_MAX_TEXT_LENGTH 256
#endif

#ifndef MG_CONFIG_HIGHL_RENDERER_FONT_HEIGHT
    #define MG_CONFIG_HIGHL_RENDERER_FONT_HEIGHT 16
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_texture
{
    uint32_t width, height;
    mg_image image;
    mg_sampler sampler;
    mg_image_array image_array;
}
mg_texture;

typedef struct mg_sprite
{
    float x, y;
    float width, height;
    mg_texture *texture;
}
mg_sprite;

typedef struct mg_font
{
    stbtt_bakedchar cdata[96];
    uint8_t bitmap[1024 * 1024];
    mg_texture *texture;
}
mg_font;

typedef struct mg_mesh
{
    mg_vertex_buffer vertex_buffer;
    mg_index_buffer index_buffer;
    uint32_t index_count;
}
mg_mesh;

typedef struct mg_mesh_vertex_2d
{
    mg_vec2 position;
    mg_vec2 texture_coord;
}
mg_mesh_vertex_2d;

typedef struct mg_mesh_vertex_3d
{
    mg_vec3 position;
    mg_vec3 normal;
    mg_vec2 texture_coord;
}
mg_mesh_vertex_3d;

MG_API mg_texture  *mg_highl_renderer_create_texture                (uint32_t width, uint32_t height, mg_sampler_filter filter, mg_pixel_format format, void *data);
MG_API mg_texture  *mg_highl_renderer_create_texture_from_file      (const char *file_name, mg_sampler_filter filter, mg_pixel_format format);
MG_API void         mg_highl_renderer_destroy_texture               (mg_texture *texture);

MG_API mg_font     *mg_highl_renderer_create_font_from_data         (void *ttf_data);
MG_API mg_font     *mg_highl_renderer_create_font_from_file         (const char *file_name);
MG_API void         mg_highl_renderer_destroy_font                  (mg_font *font);

MG_API mg_mesh     *mg_highl_renderer_create_mesh_2d                (mg_mesh_vertex_2d *vertices, uint32_t vertices_count, uint32_t *indices, uint32_t index_count);
MG_API mg_mesh     *mg_highl_renderer_create_mesh_3d                (mg_mesh_vertex_3d *vertices, uint32_t vertices_count, uint32_t *indices, uint32_t index_count);
MG_API void         mg_highl_renderer_destroy_mesh                  (mg_mesh *mesh);

#ifdef __cplusplus
}
#endif