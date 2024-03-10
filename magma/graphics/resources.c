#include "resources.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION 
#include <stb_truetype.h>

mg_texture_t *mg_graphics_create_texture(uint32_t width, uint32_t height, void *data)
{
    mg_texture_t *texture = (mg_texture_t*)malloc(sizeof(mg_texture_t));
    texture->width = width;
    texture->height = height;

    mg_image_create_info_t create_info = {
        .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
        .type = MG_IMAGE_TYPE_2D,
        .width = width,
        .height = height,
    };

    texture->image = mg_rhi_renderer_create_image(&create_info);

    mg_sampler_create_info_t sampler_create_info = {
        .mag_filter = MG_SAMPLER_FILTER_NEAREST,
        .min_filter = MG_SAMPLER_FILTER_NEAREST,
        .address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
    };

    texture->sampler = mg_rhi_renderer_create_sampler(&sampler_create_info);

    mg_image_write_info_t texture_image_write_info = {
        .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
        .width = width,
        .height = height,
        .data = data
    };

    mg_rhi_renderer_write_image(texture->image, &texture_image_write_info);
    mg_rhi_renderer_update_image(texture->image, texture->sampler, 0);

    return texture;
}

mg_texture_t *mg_graphics_create_texture_from_file(const char *file_name)
{
    int32_t width, height, channels;
    uint8_t *data = stbi_load(file_name, &width, &height, &channels, STBI_rgb_alpha);
    mg_texture_t *texture = mg_graphics_create_texture(width, height, data);
    stbi_image_free(data);
    return texture;
}

void mg_graphics_destroy_texture(mg_texture_t *texture)
{
    mg_rhi_renderer_destroy_image(texture->image);
    mg_rhi_renderer_destroy_sampler(texture->sampler);
    free(texture);
}

mg_font_t *mg_graphics_create_font(void *ttf_data)
{
    mg_font_t *font = (mg_font_t*)malloc(sizeof(mg_font_t));
    stbtt_BakeFontBitmap(ttf_data, 0, MG_CONFIG_GRAPHICS_FONT_HEIGHT, font->bitmap, 1024, 1024, 32, 96, font->cdata);
    font->texture = mg_graphics_create_texture(512, 512, font->bitmap);
    return font;
}

mg_font_t *mg_graphics_create_font_from_file(const char *file_name)
{
    long size;
    uint8_t *buffer;

    FILE* file = fopen(file_name, "rb");
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(size);

    fread(buffer, size, 1, file);
    fclose(file);

    mg_font_t *font = mg_graphics_create_font(buffer);

    free(buffer);

    return font;
}

void mg_graphics_destroy_font(mg_font_t *font)
{
    mg_graphics_destroy_texture(font->texture);
    free(font);
}

mg_mesh_t* mg_graphics_create_mesh(mg_mesh_vertex_t *vertices, uint32_t vertices_count, uint32_t *indices, uint32_t index_count)
{
    mg_mesh_t* mesh = (mg_mesh_t*)malloc(sizeof(mg_mesh_t));
    mesh->index_count = index_count;
    mesh->vertex_buffer = mg_rhi_renderer_create_vertex_buffer(vertices_count * sizeof(mg_mesh_vertex_t), vertices);
    mesh->index_buffer = mg_rhi_renderer_create_index_buffer(index_count * sizeof(uint32_t), indices);
    return mesh;
}

void mg_graphics_destroy_mesh(mg_mesh_t* mesh)
{
    mg_rhi_renderer_destroy_vertex_buffer(mesh->vertex_buffer);
    mg_rhi_renderer_destroy_index_buffer(mesh->index_buffer);
    free(mesh);
}