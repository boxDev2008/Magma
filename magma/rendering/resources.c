#include "resources.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION 
#include <stb_truetype.h>

mg_texture *mg_highl_renderer_create_texture(uint32_t width, uint32_t height, mg_sampler_filter filter, mg_pixel_format format, void *data)
{
    mg_texture *texture = (mg_texture*)malloc(sizeof(mg_texture));
    texture->width = width;
    texture->height = height;

    mg_image_create_info create_info = {
        .format = format,
        .type = MG_IMAGE_TYPE_2D,
        .width = width,
        .height = height,
    };

    texture->image = mg_lowl_renderer_create_image(&create_info);

    mg_sampler_create_info sampler_create_info = {
        .mag_filter = filter,
        .min_filter = filter,
        .address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
    };

    texture->sampler = mg_lowl_renderer_create_sampler(&sampler_create_info);

    mg_image_write_info texture_image_write_info = {
        .format = format,
        .width = width,
        .height = height,
        .data = data
    };

    mg_lowl_renderer_update_image(texture->image, &texture_image_write_info);

    texture->image_array = mg_lowl_renderer_create_image_array();
    mg_lowl_renderer_update_image_array(texture->image_array, &texture->image, &texture->sampler, 1);

    return texture;
}

mg_texture *mg_highl_renderer_create_texture_from_file(const char *file_name, mg_sampler_filter filter, mg_pixel_format format)
{
    int32_t width, height, channels;
    uint8_t *data = stbi_load(file_name, &width, &height, &channels, STBI_rgb_alpha);
	if (!data)
	{
		printf("Failed to load texture from file %s", file_name);
		return NULL;
	}
    mg_texture *texture = mg_highl_renderer_create_texture(width, height, filter, format, data);
    stbi_image_free(data);
    return texture;
}

void mg_highl_renderer_destroy_texture(mg_texture *texture)
{
    mg_lowl_renderer_destroy_image(texture->image);
    mg_lowl_renderer_destroy_sampler(texture->sampler);
    mg_lowl_renderer_destroy_image_array(texture->image_array);
    free(texture);
}

mg_font *mg_highl_renderer_create_font_from_data(void *ttf_data)
{
    mg_font *font = (mg_font*)malloc(sizeof(mg_font));
    stbtt_BakeFontBitmap(ttf_data, 0, MG_CONFIG_HIGHL_RENDERER_FONT_HEIGHT * 4, font->bitmap, 1024, 1024, 32, 96, font->cdata);
    font->texture = mg_highl_renderer_create_texture(512, 512, MG_SAMPLER_FILTER_NEAREST, MG_PIXEL_FORMAT_R8G8B8A8_SRGB, font->bitmap);
    return font;
}

mg_font *mg_highl_renderer_create_font_from_file(const char *file_name)
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

    mg_font *font = mg_highl_renderer_create_font_from_data(buffer);

    free(buffer);

    return font;
}

void mg_highl_renderer_destroy_font(mg_font *font)
{
    mg_highl_renderer_destroy_texture(font->texture);
    free(font);
}

mg_mesh *mg_highl_renderer_create_mesh_2d(mg_mesh_vertex_2d *vertices, uint32_t vertices_count, uint32_t *indices, uint32_t index_count)
{
    mg_mesh* mesh = (mg_mesh*)malloc(sizeof(mg_mesh));
    mesh->index_count = index_count;
    mesh->vertex_buffer = mg_lowl_renderer_create_vertex_buffer(vertices_count * sizeof(mg_mesh_vertex_2d), vertices);
    mesh->index_buffer = mg_lowl_renderer_create_index_buffer(index_count * sizeof(uint32_t), indices);
    return mesh;
}

mg_mesh *mg_highl_renderer_create_mesh_3d(mg_mesh_vertex_3d *vertices, uint32_t vertices_count, uint32_t *indices, uint32_t index_count)
{
    mg_mesh* mesh = (mg_mesh*)malloc(sizeof(mg_mesh));
    mesh->index_count = index_count;
    mesh->vertex_buffer = mg_lowl_renderer_create_vertex_buffer(vertices_count * sizeof(mg_mesh_vertex_3d), vertices);
    mesh->index_buffer = mg_lowl_renderer_create_index_buffer(index_count * sizeof(uint32_t), indices);
    return mesh;
}

void mg_highl_renderer_destroy_mesh(mg_mesh *mesh)
{
    mg_lowl_renderer_destroy_vertex_buffer(mesh->vertex_buffer);
    mg_lowl_renderer_destroy_index_buffer(mesh->index_buffer);
    free(mesh);
}