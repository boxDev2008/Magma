#include "2d.h"

#include "math/mat4.h"

#include "rendering/vulkan/shaders/post_process/vert.spv.h"
#include "rendering/vulkan/shaders/post_process/frag.spv.h"

#include "rendering/vulkan/shaders/quad/vert.spv.h"
#include "rendering/vulkan/shaders/quad/frag.spv.h"

#include "rendering/vulkan/shaders/sprite/vert.spv.h"
#include "rendering/vulkan/shaders/sprite/frag.spv.h"

#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

typedef struct mg_graphics_2d_data mg_graphics_2d_data_t;
struct mg_graphics_2d_data
{
    mg_index_buffer_t index_buffer;

    struct
    {
        mg_pipeline_t pipeline;
        mg_vertex_buffer_t vertex_buffer;
        mg_render_pass_t render_pass;
        mg_framebuffer_t framebuffer;
        mg_image_t image;
        mg_sampler_t sampler;

        struct
        {
            mg_mat4_t projection;
        }
        ubo;

        mg_uniform_buffer_t uniform_buffer;
    }
    world_data;

    struct
    {
        mg_pipeline_t pipeline;
        mg_vertex_buffer_t vertex_buffer;
        
        struct
        {
            mg_mat4_t model;
            mg_vec4_t color;
        }
        push_constant;
    }
    quad_data;

    struct
    {
        mg_pipeline_t pipeline;
        mg_vertex_buffer_t vertex_buffer;
        
        struct
        {
            mg_mat4_t model;
            mg_vec4_t color;
            mg_vec4_t rect;
        }
        push_constant;
    }
    sprite_data;

    struct
    {
        uint32_t bind_id;
    }
    frame_data;

    int32_t width, height;
};

static mg_graphics_2d_data_t graphics_data;

void mg_graphics_2d_initialize(int32_t width, int32_t height)
{
    graphics_data.width = width;
    graphics_data.height = height;

    uint32_t indices[6] = {
        0, 1, 3, 1, 2, 3
    };

    graphics_data.index_buffer =
        mg_rhi_renderer_create_index_buffer(sizeof(indices), indices);

    // World Data
    {
        mg_shader_source_t vertex_shader = {
            .code = VULKAN_POST_PROCESS_VERT,
            .code_size = sizeof(VULKAN_POST_PROCESS_VERT)
        };

        mg_shader_source_t fragment_shader = {
            .code = VULKAN_POST_PROCESS_FRAG,
            .code_size = sizeof(VULKAN_POST_PROCESS_FRAG)
        };

        mg_vertex_attribute_info_t vertex_attributes[2] = {
            {
                .location = 0,
                .offset = 0,
                .format = MG_VERTEX_FORMAT_FLOAT2
            },
            {
                .location = 1,
                .offset = 2 * sizeof(float),
                .format = MG_VERTEX_FORMAT_FLOAT2
            }
        };

        mg_vertex_layout_info_t vertex_layout = {
            .stride = 4 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 2
        };

        mg_pipeline_create_info_t pipeline_create_info = {
            .vertex_shader = &vertex_shader,
            .fragment_shader = &fragment_shader,

            vertex_layout = vertex_layout,

            .polygon_mode = MG_POLYGON_MODE_FILL,
            .cull_mode = MG_CULL_MODE_BACK,
            .front_face = MG_FRONT_FACE_CW,

            .color_blend = {
                .blend_enabled = false,
                .src_color_blend_factor = MG_BLEND_FACTOR_ONE,
                .dst_color_blend_factor = MG_BLEND_FACTOR_ZERO,
                .color_blend_op = MG_BLEND_OP_ADD,
                .src_alpha_blend_factor = MG_BLEND_FACTOR_ONE,
                .dst_alpha_blend_factor = MG_BLEND_FACTOR_ZERO,
                .alpha_blend_op = MG_BLEND_OP_ADD
            }
        };

        graphics_data.world_data.pipeline =
            mg_rhi_renderer_create_pipeline(&pipeline_create_info);
        
        const float vertices[16] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 1.0f
        };
        
        graphics_data.world_data.vertex_buffer =
            mg_rhi_renderer_create_vertex_buffer(sizeof(vertices), vertices);

        mg_render_pass_create_info_t mrender_pass_create_info = {
            .color_attachment = {
                .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
                .load_op = MG_ATTACHMENT_LOAD_OP_CLEAR,
                .store_op = MG_ATTACHMENT_STORE_OP_STORE
            }
        };

        graphics_data.world_data.render_pass =
            mg_rhi_renderer_create_render_pass(&mrender_pass_create_info);

        mg_image_create_info_t image_create_info = {
            .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
            .type = MG_IMAGE_TYPE_2D,
            .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
            .width = width,
            .height = height
        };

        graphics_data.world_data.image =
            mg_rhi_renderer_create_image(&image_create_info);
        
        mg_sampler_create_info_t sampler_create_info = {
            .mag_filter = MG_SAMPLER_FILTER_NEAREST,
            .min_filter = MG_SAMPLER_FILTER_NEAREST,
            .address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
        };
        
        graphics_data.world_data.sampler =
            mg_rhi_renderer_create_sampler(&sampler_create_info);
        
        mg_rhi_renderer_update_image(graphics_data.world_data.image, graphics_data.world_data.sampler);

        mg_framebuffer_create_info_t framebuffer_create_info = {
            .width = width,
            .height = height,
            .render_pass = graphics_data.world_data.render_pass,
            .image = graphics_data.world_data.image
        };
        graphics_data.world_data.framebuffer =
            mg_rhi_renderer_create_framebuffer(&framebuffer_create_info);

        graphics_data.world_data.uniform_buffer =
            mg_rhi_renderer_create_uniform_buffer(sizeof(graphics_data.world_data.ubo));
    }

    // Quad Data
    {
        mg_shader_source_t vertex_shader = {
            .code = VULKAN_QUAD_VERT,
            .code_size = sizeof(VULKAN_QUAD_VERT)
        };

        mg_shader_source_t fragment_shader = {
            .code = VULKAN_QUAD_FRAG,
            .code_size = sizeof(VULKAN_QUAD_FRAG)
        };

        mg_vertex_attribute_info_t vertex_attributes[1] = {
            {
                .location = 0,
                .offset = 0,
                .format = MG_VERTEX_FORMAT_FLOAT2
            }
        };

        mg_vertex_layout_info_t vertex_layout = {
            .stride = 2 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 1
        };

        mg_pipeline_create_info_t pipeline_create_info = {
            .vertex_shader = &vertex_shader,
            .fragment_shader = &fragment_shader,

            vertex_layout = vertex_layout,

            .polygon_mode = MG_POLYGON_MODE_FILL,
            .cull_mode = MG_CULL_MODE_BACK,
            .front_face = MG_FRONT_FACE_CW,

            .color_blend = {
                .blend_enabled = true,
                .src_color_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
                .dst_color_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = MG_BLEND_OP_ADD,
                .src_alpha_blend_factor = MG_BLEND_FACTOR_ONE,
                .dst_alpha_blend_factor = MG_BLEND_FACTOR_ZERO,
                .alpha_blend_op = MG_BLEND_OP_ADD
            },

            .has_push_constants = true,
            .push_constants_size = sizeof(graphics_data.quad_data.push_constant)
        };

        graphics_data.quad_data.pipeline =
            mg_rhi_renderer_create_pipeline(&pipeline_create_info);

        const float vertices[8] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f
        };
        
        graphics_data.quad_data.vertex_buffer =
            mg_rhi_renderer_create_vertex_buffer(sizeof(vertices), vertices);
    }

    // Sprite Data
    {
        mg_shader_source_t vertex_shader = {
            .code = VULKAN_SPRITE_VERT,
            .code_size = sizeof(VULKAN_SPRITE_VERT)
        };

        mg_shader_source_t fragment_shader = {
            .code = VULKAN_SPRITE_FRAG,
            .code_size = sizeof(VULKAN_SPRITE_FRAG)
        };

        mg_vertex_attribute_info_t vertex_attributes[2] = {
            {
                .location = 0,
                .offset = 0,
                .format = MG_VERTEX_FORMAT_FLOAT2
            },
            {
                .location = 1,
                .offset = 2 * sizeof(float),
                .format = MG_VERTEX_FORMAT_FLOAT2
            }
        };

        mg_vertex_layout_info_t vertex_layout = {
            .stride = 4 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 2
        };

        mg_pipeline_create_info_t pipeline_create_info = {
            .vertex_shader = &vertex_shader,
            .fragment_shader = &fragment_shader,

            vertex_layout = vertex_layout,

            .polygon_mode = MG_POLYGON_MODE_FILL,
            .cull_mode = MG_CULL_MODE_BACK,
            .front_face = MG_FRONT_FACE_CW,

            .color_blend = {
                .blend_enabled = true,
                .src_color_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
                .dst_color_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = MG_BLEND_OP_ADD,
                .src_alpha_blend_factor = MG_BLEND_FACTOR_ONE,
                .dst_alpha_blend_factor = MG_BLEND_FACTOR_ZERO,
                .alpha_blend_op = MG_BLEND_OP_ADD
            },

            .has_push_constants = true,
            .push_constants_size = sizeof(graphics_data.sprite_data.push_constant)
        };

        graphics_data.sprite_data.pipeline =
            mg_rhi_renderer_create_pipeline(&pipeline_create_info);

        const float vertices[16] = {
            -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 1.0f
        };
        
        graphics_data.sprite_data.vertex_buffer =
            mg_rhi_renderer_create_vertex_buffer(sizeof(vertices), vertices);
    }
}

void mg_graphics_2d_shutdown(void)
{
    mg_rhi_renderer_wait();

    // World Data
    {
        mg_rhi_renderer_destroy_uniform_buffer(graphics_data.world_data.uniform_buffer);
        mg_rhi_renderer_destroy_framebuffer(graphics_data.world_data.framebuffer);
        mg_rhi_renderer_destroy_image(graphics_data.world_data.image);
        mg_rhi_renderer_destroy_sampler(graphics_data.world_data.sampler);
        mg_rhi_renderer_destroy_render_pass(graphics_data.world_data.render_pass);
        mg_rhi_renderer_destroy_vertex_buffer(graphics_data.world_data.vertex_buffer);
        mg_rhi_renderer_destroy_pipeline(graphics_data.world_data.pipeline);
    }

    // Quad Data
    {
        mg_rhi_renderer_destroy_vertex_buffer(graphics_data.quad_data.vertex_buffer);
        mg_rhi_renderer_destroy_pipeline(graphics_data.quad_data.pipeline);
    }

    // Sprite Data
    {
        mg_rhi_renderer_destroy_vertex_buffer(graphics_data.sprite_data.vertex_buffer);
        mg_rhi_renderer_destroy_pipeline(graphics_data.sprite_data.pipeline);
    }

    mg_rhi_renderer_destroy_index_buffer(graphics_data.index_buffer);
}

void mg_graphics_2d_resize(int32_t width, int32_t height)
{
    mg_rhi_renderer_destroy_framebuffer(graphics_data.world_data.framebuffer);
    mg_rhi_renderer_destroy_image(graphics_data.world_data.image);

    graphics_data.width = width;
    graphics_data.height = height;

    mg_image_create_info_t image_create_info = {
        .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
        .type = MG_IMAGE_TYPE_2D,
        .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
        .width = width,
        .height = height
    };

    graphics_data.world_data.image =
        mg_rhi_renderer_create_image(&image_create_info);

    mg_rhi_renderer_update_image(graphics_data.world_data.image, graphics_data.world_data.sampler);

    mg_framebuffer_create_info_t framebuffer_create_info = {
        .width = width,
        .height = height,
        .render_pass = graphics_data.world_data.render_pass,
        .image = graphics_data.world_data.image
    };

    graphics_data.world_data.framebuffer =
        mg_rhi_renderer_create_framebuffer(&framebuffer_create_info);
}

void mg_graphics_2d_begin(void)
{
    graphics_data.frame_data.bind_id = 0;
}

void mg_graphics_2d_end(void)
{
    mg_render_pass_begin_info_t render_pass_begin_info = {
        .render_area = (mg_vec4_t) { 0.0f, 0.0f, graphics_data.width, graphics_data.height },
        .clear_value = (mg_vec4_t) {0.0f, 0.0f, 0.0f, 1.0f }
    };

    mg_rhi_renderer_begin_default_render_pass(&render_pass_begin_info);
    mg_rhi_renderer_viewport(graphics_data.width, graphics_data.height);

    mg_rhi_renderer_bind_pipeline(graphics_data.world_data.pipeline);
    mg_rhi_renderer_bind_vertex_buffer(graphics_data.world_data.vertex_buffer);
    mg_rhi_renderer_bind_uniform_buffer(graphics_data.world_data.uniform_buffer, graphics_data.world_data.pipeline);
    mg_rhi_renderer_bind_image(graphics_data.world_data.image, graphics_data.world_data.pipeline);
    mg_rhi_renderer_draw_indexed(6, 0);

    mg_rhi_renderer_end_render_pass();
}

void mg_graphics_2d_begin_world(mg_world_info_t *world_info)
{
    mg_render_pass_begin_info_t render_pass_begin_info = {
        .render_area = (mg_vec4_t) {0.0f, 0.0f, graphics_data.width, graphics_data.height},
        .clear_value = (mg_vec4_t) {0.0f, 0.0f, 0.0f, 1.0f}
    };

    mg_rhi_renderer_begin_render_pass(graphics_data.world_data.render_pass,
        graphics_data.world_data.framebuffer, &render_pass_begin_info);

    mg_rhi_renderer_viewport(graphics_data.width, graphics_data.height);

    mg_mat4_t projection;

    const int32_t width = graphics_data.width;
    const int32_t height = graphics_data.height;

    projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);

    projection = mg_mat4_translate(projection, (mg_vec3_t) { -world_info->camera_position.x, -world_info->camera_position.y, 0.0f });
    projection = mg_mat4_scale(projection, (mg_vec3_t) { world_info->camera_zoom.x, world_info->camera_zoom.y, 1.0f });

    graphics_data.world_data.ubo.projection = projection;
    mg_rhi_renderer_update_uniform_buffer(graphics_data.world_data.uniform_buffer,
        sizeof(graphics_data.world_data.ubo), &graphics_data.world_data.ubo);
}

void mg_graphics_2d_end_world(void)
{
    mg_rhi_renderer_end_render_pass();
}

void mg_graphics_2d_draw_rect(mg_vec2_t position, mg_vec2_t scale, mg_vec4_t color)
{
    if (graphics_data.frame_data.bind_id != 1)
    {
        mg_rhi_renderer_bind_pipeline(graphics_data.quad_data.pipeline);
        mg_rhi_renderer_bind_vertex_buffer(graphics_data.quad_data.vertex_buffer);
        mg_rhi_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT32);
        graphics_data.frame_data.bind_id = 1;
    }

    mg_mat4_t model = mg_mat4_identity();
    model = mg_mat4_scale(model, (mg_vec3_t){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3_t){ position.x, position.y, 0.0f });

    graphics_data.quad_data.push_constant.model = model;
    graphics_data.quad_data.push_constant.color = color;

    mg_rhi_renderer_push_constants(graphics_data.quad_data.pipeline,
        sizeof(graphics_data.quad_data.push_constant), &graphics_data.quad_data.push_constant);

    mg_rhi_renderer_draw_indexed(6, 0);
}

void mg_graphics_2d_draw_rotated_rect(mg_vec2_t position, mg_vec2_t scale, float rotation, mg_vec4_t color)
{
    if (graphics_data.frame_data.bind_id != 1)
    {
        mg_rhi_renderer_bind_pipeline(graphics_data.quad_data.pipeline);
        mg_rhi_renderer_bind_vertex_buffer(graphics_data.quad_data.vertex_buffer);
        mg_rhi_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT32);
        graphics_data.frame_data.bind_id = 1;
    }

    mg_mat4_t model = mg_mat4_identity();
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_scale(model, (mg_vec3_t){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3_t){ position.x, position.y, 0.0f });

    graphics_data.quad_data.push_constant.model = model;
    graphics_data.quad_data.push_constant.color = color;

    mg_rhi_renderer_push_constants(graphics_data.quad_data.pipeline,
        sizeof(graphics_data.quad_data.push_constant), &graphics_data.quad_data.push_constant);

    mg_rhi_renderer_draw_indexed(6, 0);
}

void mg_graphics_2d_draw_sprite(mg_vec2_t position, mg_vec2_t scale, mg_vec4_t color, mg_sprite_t *sprite)
{
    if (graphics_data.frame_data.bind_id != 2)
    {
        mg_rhi_renderer_bind_pipeline(graphics_data.sprite_data.pipeline);
        mg_rhi_renderer_bind_vertex_buffer(graphics_data.sprite_data.vertex_buffer);
        mg_rhi_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT32);
        graphics_data.frame_data.bind_id = 2;
    }

    mg_rhi_renderer_bind_image(sprite->texture->image, graphics_data.sprite_data.pipeline);

    mg_mat4_t model = mg_mat4_identity();
    model = mg_mat4_scale(model, (mg_vec3_t){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3_t){ position.x, position.y, 0.0f });

    graphics_data.sprite_data.push_constant.model = model;
    graphics_data.sprite_data.push_constant.color = color;
    graphics_data.sprite_data.push_constant.rect =
        (mg_vec4_t) { sprite->x, sprite->y, sprite->width, sprite->height };

    mg_rhi_renderer_push_constants(graphics_data.sprite_data.pipeline,
        sizeof(graphics_data.sprite_data.push_constant), &graphics_data.sprite_data.push_constant);

    mg_rhi_renderer_draw_indexed(6, 0);
}

void mg_graphics_2d_draw_rotated_sprite(mg_vec2_t position, mg_vec2_t scale, float rotation, mg_vec4_t color, mg_sprite_t *sprite)
{
    if (graphics_data.frame_data.bind_id != 2)
    {
        mg_rhi_renderer_bind_pipeline(graphics_data.sprite_data.pipeline);
        mg_rhi_renderer_bind_vertex_buffer(graphics_data.sprite_data.vertex_buffer);
        mg_rhi_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT32);
        graphics_data.frame_data.bind_id = 2;
    }

    mg_rhi_renderer_bind_image(sprite->texture->image, graphics_data.sprite_data.pipeline);

    mg_mat4_t model = mg_mat4_identity();
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_scale(model, (mg_vec3_t){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3_t){ position.x, position.y, 0.0f });

    graphics_data.sprite_data.push_constant.model = model;
    graphics_data.sprite_data.push_constant.color = color;
    graphics_data.sprite_data.push_constant.rect =
        (mg_vec4_t) { sprite->x, sprite->y, sprite->width, sprite->height };

    mg_rhi_renderer_push_constants(graphics_data.sprite_data.pipeline,
        sizeof(graphics_data.sprite_data.push_constant), &graphics_data.sprite_data.push_constant);

    mg_rhi_renderer_draw_indexed(6, 0);
}

mg_texture_t *mg_graphics_2d_create_texture(uint32_t width, uint32_t height, void *data)
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
    mg_rhi_renderer_update_image(texture->image, texture->sampler);

    return texture;
}

mg_texture_t *mg_graphics_2d_create_texture_from_file(const char *file_name)
{
    int32_t width, height, channels;
    uint8_t *data = stbi_load(file_name, &width, &height, &channels, STBI_rgb_alpha);
    mg_texture_t *texture = mg_graphics_2d_create_texture(width, height, data);
    stbi_image_free(data);
    return texture;
}

void mg_graphics_2d_destroy_texture(mg_texture_t *texture)
{
    mg_rhi_renderer_destroy_image(texture->image);
    mg_rhi_renderer_destroy_sampler(texture->sampler);
    free(texture);
}

mg_sprite_t *mg_graphics_2d_create_sprite(uint32_t x, uint32_t y, uint32_t width, uint32_t height, mg_texture_t *texture)
{
    mg_sprite_t *sprite = (mg_sprite_t*)malloc(sizeof(mg_sprite_t));
    sprite->texture = texture;
    sprite->x = x;
    sprite->y = y;
    sprite->width = width;
    sprite->height = height;
    return sprite;
}

void mg_graphics_2d_destroy_sprite(mg_sprite_t *sprite)
{
    free(sprite);
}