#include "graphics.h"

#include "math/mat4.h"
#include "math/math.h"

#include "rendering/vulkan/shaders/2d/world/vert.spv.h"
#include "rendering/vulkan/shaders/2d/world/frag.spv.h"

#include "rendering/vulkan/shaders/2d/nonlit/vert.spv.h"
#include "rendering/vulkan/shaders/2d/nonlit/frag.spv.h"

#include "rendering/vulkan/shaders/2d/quad/vert.spv.h"
#include "rendering/vulkan/shaders/2d/quad/frag.spv.h"

#include "rendering/vulkan/shaders/2d/sprite/vert.spv.h"
#include "rendering/vulkan/shaders/2d/sprite/frag.spv.h"

#include "rendering/vulkan/shaders/2d/text/vert.spv.h"
#include "rendering/vulkan/shaders/2d/text/frag.spv.h"

#include "rendering/vulkan/shaders/3d/world/vert.spv.h"
#include "rendering/vulkan/shaders/3d/world/frag.spv.h"

#include "rendering/vulkan/shaders/3d/mesh/vert.spv.h"
#include "rendering/vulkan/shaders/3d/mesh/frag.spv.h"

#include "rendering/opengl/shaders/2d/world/vert.h"
#include "rendering/opengl/shaders/2d/world/frag.h"

#include "rendering/opengl/shaders/2d/nonlit/vert.h"
#include "rendering/opengl/shaders/2d/nonlit/frag.h"

#include "rendering/opengl/shaders/2d/quad/vert.h"
#include "rendering/opengl/shaders/2d/quad/frag.h"

#include "rendering/opengl/shaders/2d/sprite/vert.h"
#include "rendering/opengl/shaders/2d/sprite/frag.h"

#include "rendering/opengl/shaders/2d/text/vert.h"
#include "rendering/opengl/shaders/2d/text/frag.h"

#include "rendering/opengl/shaders/3d/world/vert.h"
#include "rendering/opengl/shaders/3d/world/frag.h"

#include "rendering/opengl/shaders/3d/mesh/vert.h"
#include "rendering/opengl/shaders/3d/mesh/frag.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MG_GRAPHICS_2D_MAX_LIGHT_COUNT 16

#define MG_GRAPHICS_2D_GLOBAL_UNIFORM_BUFFER_WORLD_INDEX 0
#define MG_GRAPHICS_2D_GLOBAL_UNIFORM_BUFFER_NONLIT_INDEX 1

typedef struct mg_shader_resources
{
    void
    *world_2d_vert, *world_2d_frag,
    *nonlit_2d_vert, *nonlit_2d_frag,
    *world_3d_vert, *world_3d_frag,
    *quad_vert, *quad_frag,
    *sprite_vert, *sprite_frag,
    *text_vert, *text_frag,
    *mesh_vert, *mesh_frag;

    size_t
    world_2d_vert_size, world_2d_frag_size,
    nonlit_2d_vert_size, nonlit_2d_frag_size,
    world_3d_vert_size, world_3d_frag_size,
    quad_vert_size, quad_frag_size,
    sprite_vert_size, sprite_frag_size,
    text_vert_size, text_frag_size,
    mesh_vert_size, mesh_frag_size;
}
mg_shader_resources_t;

typedef struct mg_graphics_data
{
    mg_index_buffer_t index_buffer;

    mg_vertex_buffer_t screen_vertex_buffer;
    mg_sampler_t screen_sampler;

    mg_render_layer_t *layers[MG_CONFIG_GRAPHICS_MAX_RENDER_LAYERS];
    uint32_t layer_count;

    struct
    {
        mg_mat4_t view_projection;
    }
    global_ubo;

    struct
    {
        mg_vec4_t light_transform[MG_GRAPHICS_2D_MAX_LIGHT_COUNT];
        mg_vec4_t light_colors[MG_GRAPHICS_2D_MAX_LIGHT_COUNT];
        mg_vec4_t global_light_data;
        mg_vec4_t shadow_data;
        mg_vec2_t resolution;
        int32_t light_count;
        int32_t flag_enable_vignette;
        int32_t flag_enable_lights;
        int32_t flag_enable_shadows;
    }
    world_2d_ubo;

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
        mg_pipeline_t pipeline;

        mg_dynamic_vertex_buffer_t vertex_buffers[MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS];
        mg_dynamic_index_buffer_t index_buffers[MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS];

        float vertices[MG_CONFIG_GRAPHICS_MAX_TEXT_LENGTH * 16];
        uint32_t indices[MG_CONFIG_GRAPHICS_MAX_TEXT_LENGTH * 6];

        struct
        {
            mg_mat4_t model;
            mg_vec4_t color;
        }
        push_constant;
    }
    text_data;

    struct
    {
        mg_pipeline_t pipeline;

        struct
        {
            mg_mat4_t model;
            mg_vec4_t color;
        }
        push_constant;
    }
    mesh_data;

    struct
    {
        uint16_t pipeline_bind_id;
        uint16_t vertex_buffer_bind_id;
        uint16_t index_buffer_bind_id;
        uint32_t text_index;
    }
    frame_data;

    mg_shader_resources_t shader_resources;

    mg_pixel_format_t format;
    int32_t width, height;
}
mg_graphics_data_t;

static mg_graphics_data_t graphics_data;

mg_render_layer_t *mg_graphics_create_render_layer(mg_shader_source_t vertex_shader, mg_shader_source_t fragment_shader,
bool has_depth_stencil_attachment, size_t global_ub_size, size_t internal_ub_size)
{
    mg_render_layer_t *layer = (mg_render_layer_t*)calloc(1, sizeof(mg_render_layer_t));
    mg_render_pass_create_info_t render_pass_create_info = {
        .color_attachment = {
            .format = graphics_data.format
        }
    };

    mg_image_create_info_t color_attachment_create_info = {
        .format = graphics_data.format,
        .type = MG_IMAGE_TYPE_2D,
        .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
        .width = graphics_data.width,
        .height = graphics_data.height
    };

    layer->color_attachment =
        mg_rhi_renderer_create_image(&color_attachment_create_info);

    mg_rhi_renderer_update_image(layer->color_attachment, graphics_data.screen_sampler, 0);

    mg_framebuffer_create_info_t framebuffer_create_info = {
        .width = graphics_data.width,
        .height = graphics_data.height,
        .color_attachment = layer->color_attachment
    };

    if (has_depth_stencil_attachment)
    {
        render_pass_create_info.depth_stencil_attachment = (mg_attachment_info_t){
            .format = MG_PIXEL_FORMAT_D32_SFLOAT
        };
        render_pass_create_info.has_depth_stencil_attachment = true;

        mg_image_create_info_t depth_stencil_attachment_create_info = {
            .format = MG_PIXEL_FORMAT_D32_SFLOAT,
            .type = MG_IMAGE_TYPE_2D,
            .usage = MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT,
            .width = graphics_data.width,
            .height = graphics_data.height
        };

        layer->depth_stencil_attachment =
            mg_rhi_renderer_create_image(&depth_stencil_attachment_create_info);

        framebuffer_create_info.depth_stencil_attachment = layer->depth_stencil_attachment;
    }

    layer->render_pass =
        mg_rhi_renderer_create_render_pass(&render_pass_create_info);

    framebuffer_create_info.render_pass = layer->render_pass;

    layer->framebuffer =
        mg_rhi_renderer_create_framebuffer(&framebuffer_create_info);

    const mg_vertex_attribute_info_t vertex_attributes[2] = {
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
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,

        vertex_layout = vertex_layout,

        .polygon_mode = MG_POLYGON_MODE_FILL,
        .cull_mode = MG_CULL_MODE_NONE,
        .front_face = MG_FRONT_FACE_CW,

        .color_blend = {
            .blend_enabled = true,
            .src_color_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
            .dst_color_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .color_blend_op = MG_BLEND_OP_ADD,
            .src_alpha_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
            .dst_alpha_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .alpha_blend_op = MG_BLEND_OP_ADD
        },

        .render_pass = layer->render_pass
    };

    layer->pipeline =
        mg_rhi_renderer_create_pipeline(&pipeline_create_info);
    
    layer->global_ub = mg_rhi_renderer_create_uniform_buffer(global_ub_size);

    if (internal_ub_size > 0)
        layer->internal_ub = mg_rhi_renderer_create_uniform_buffer(internal_ub_size);

    return layer;
}

void mg_graphics_destroy_render_layer(mg_render_layer_t *layer)
{
    mg_rhi_renderer_destroy_framebuffer(layer->framebuffer);
    mg_rhi_renderer_destroy_image(layer->color_attachment);

    if (layer->depth_stencil_attachment.internal_data)
        mg_rhi_renderer_destroy_image(layer->depth_stencil_attachment);

    mg_rhi_renderer_destroy_pipeline(layer->pipeline);
    mg_rhi_renderer_destroy_render_pass(layer->render_pass);

    mg_rhi_renderer_destroy_uniform_buffer(layer->global_ub);

    if (layer->internal_ub.internal_data)
        mg_rhi_renderer_destroy_uniform_buffer(layer->internal_ub);

    free(layer);
}

static void mg_graphics_resize_render_layer(mg_render_layer_t *layer)
{
    mg_rhi_renderer_destroy_framebuffer(layer->framebuffer);
    mg_rhi_renderer_destroy_image(layer->color_attachment);

    mg_image_create_info_t color_attachment_create_info = {
        .format = graphics_data.format,
        .type = MG_IMAGE_TYPE_2D,
        .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
        .width = graphics_data.width,
        .height = graphics_data.height
    };

    layer->color_attachment =
        mg_rhi_renderer_create_image(&color_attachment_create_info);

    mg_rhi_renderer_update_image(layer->color_attachment, graphics_data.screen_sampler, 0);

    mg_framebuffer_create_info_t framebuffer_create_info = {
        .width = graphics_data.width,
        .height = graphics_data.height,
        .render_pass = layer->render_pass,
        .color_attachment = layer->color_attachment
    };

    if (layer->depth_stencil_attachment.internal_data)
    {
        mg_image_create_info_t depth_stencil_attachment_create_info = {
            .format = MG_PIXEL_FORMAT_D32_SFLOAT,
            .type = MG_IMAGE_TYPE_2D,
            .usage = MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT,
            .width = graphics_data.width,
            .height = graphics_data.height
        };

        layer->depth_stencil_attachment =
            mg_rhi_renderer_create_image(&depth_stencil_attachment_create_info);

        framebuffer_create_info.depth_stencil_attachment = layer->depth_stencil_attachment;
    }

    layer->framebuffer =
        mg_rhi_renderer_create_framebuffer(&framebuffer_create_info);
}

void mg_graphics_add_render_layer(mg_render_layer_t *layer)
{
    graphics_data.layers[graphics_data.layer_count] = layer;
    graphics_data.layer_count++;
}

void mg_graphics_begin_render_layer(mg_render_layer_t *layer, mg_vec4_t clear_value)
{
    mg_render_pass_begin_info_t render_pass_begin_info = {
        .render_area = (mg_vec4_t) {0.0f, 0.0f, graphics_data.width, graphics_data.height},
        .clear_value = clear_value
    };

    mg_rhi_renderer_begin_render_pass(layer->render_pass,
        layer->framebuffer, &render_pass_begin_info);

    mg_rhi_renderer_viewport(0, 0, graphics_data.width, graphics_data.height);
}

void mg_graphics_end_render_layer(mg_render_layer_t *layer)
{
    mg_rhi_renderer_end_render_pass();
}

mg_render_layer_t *mg_graphics_create_nonlit_layer_2d(void)
{
    mg_shader_source_t vertex_shader = {
        .code = graphics_data.shader_resources.nonlit_2d_vert,
        .code_size = graphics_data.shader_resources.nonlit_2d_vert_size
    };

    mg_shader_source_t fragment_shader = {
        .code = graphics_data.shader_resources.nonlit_2d_frag,
        .code_size = graphics_data.shader_resources.nonlit_2d_frag_size
    };

    mg_render_layer_t *layer = mg_graphics_create_render_layer(vertex_shader, fragment_shader,
        false, sizeof(graphics_data.global_ubo), 0);

    return layer;
}

mg_render_layer_t *mg_graphics_create_world_layer_2d(void)
{
    mg_shader_source_t vertex_shader = {
        .code = graphics_data.shader_resources.world_2d_vert,
        .code_size = graphics_data.shader_resources.world_2d_vert_size
    };

    mg_shader_source_t fragment_shader = {
        .code = graphics_data.shader_resources.world_2d_frag,
        .code_size = graphics_data.shader_resources.world_2d_frag_size
    };

    mg_render_layer_t *layer = mg_graphics_create_render_layer(vertex_shader, fragment_shader,
        false, sizeof(graphics_data.global_ubo), sizeof(graphics_data.world_2d_ubo));

    return layer;
}

mg_render_layer_t *mg_graphics_create_world_layer_3d(void)
{
    mg_shader_source_t vertex_shader = {
        .code = graphics_data.shader_resources.world_3d_vert,
        .code_size = graphics_data.shader_resources.world_3d_vert_size
    };

    mg_shader_source_t fragment_shader = {
        .code = graphics_data.shader_resources.world_3d_frag,
        .code_size = graphics_data.shader_resources.world_3d_frag_size
    };

    mg_render_layer_t *layer = mg_graphics_create_render_layer(vertex_shader, fragment_shader,
        true, sizeof(graphics_data.global_ubo), 0);

    return layer;
}

void mg_graphics_initialize(mg_renderer_init_info_t *renderer_info)
{
    graphics_data.width = renderer_info->swapchain_config_info->width;
    graphics_data.height = renderer_info->swapchain_config_info->height;
    graphics_data.format = renderer_info->swapchain_config_info->format;

    graphics_data.world_2d_ubo.resolution = (mg_vec2_t){ graphics_data.width, graphics_data.height };

    graphics_data.layer_count = 0;

    mg_shader_resources_t shader_resources;

    switch (renderer_info->type)
    {
    case MG_RENDERER_TYPE_OPENGL:
        shader_resources.world_2d_vert = OPENGL_WORLD_2D_VERT;
        shader_resources.world_2d_vert_size = sizeof(OPENGL_WORLD_2D_VERT);
        shader_resources.world_2d_frag = OPENGL_WORLD_2D_FRAG;
        shader_resources.world_2d_frag_size = sizeof(OPENGL_WORLD_2D_FRAG);

        shader_resources.nonlit_2d_vert = OPENGL_NONLIT_2D_VERT;
        shader_resources.nonlit_2d_vert_size = sizeof(OPENGL_NONLIT_2D_VERT);
        shader_resources.nonlit_2d_frag = OPENGL_NONLIT_2D_FRAG;
        shader_resources.nonlit_2d_frag_size = sizeof(OPENGL_NONLIT_2D_FRAG);

        shader_resources.world_3d_vert = OPENGL_WORLD_3D_VERT;
        shader_resources.world_3d_vert_size = sizeof(OPENGL_WORLD_3D_VERT);
        shader_resources.world_3d_frag = OPENGL_WORLD_3D_FRAG;
        shader_resources.world_3d_frag_size = sizeof(OPENGL_WORLD_3D_FRAG);

        shader_resources.quad_vert = OPENGL_QUAD_VERT;
        shader_resources.quad_vert_size = sizeof(OPENGL_QUAD_VERT);
        shader_resources.quad_frag = OPENGL_QUAD_FRAG;
        shader_resources.quad_frag_size = sizeof(OPENGL_QUAD_FRAG);

        shader_resources.sprite_vert = OPENGL_SPRITE_VERT;
        shader_resources.sprite_vert_size = sizeof(OPENGL_SPRITE_VERT);
        shader_resources.sprite_frag = OPENGL_SPRITE_FRAG;
        shader_resources.sprite_frag_size = sizeof(OPENGL_SPRITE_FRAG);

        shader_resources.text_vert = OPENGL_TEXT_VERT;
        shader_resources.text_vert_size = sizeof(OPENGL_TEXT_VERT);
        shader_resources.text_frag = OPENGL_TEXT_FRAG;
        shader_resources.text_frag_size = sizeof(OPENGL_TEXT_FRAG);

        shader_resources.mesh_vert = OPENGL_MESH_VERT;
        shader_resources.mesh_vert_size = sizeof(OPENGL_MESH_VERT);
        shader_resources.mesh_frag = OPENGL_MESH_FRAG;
        shader_resources.mesh_frag_size = sizeof(OPENGL_MESH_FRAG);
        break;
    case MG_RENDERER_TYPE_VULKAN:
        shader_resources.world_2d_vert = VULKAN_WORLD_2D_VERT;
        shader_resources.world_2d_vert_size = sizeof(VULKAN_WORLD_2D_VERT);
        shader_resources.world_2d_frag = VULKAN_WORLD_2D_FRAG;
        shader_resources.world_2d_frag_size = sizeof(VULKAN_WORLD_2D_FRAG);

        shader_resources.nonlit_2d_vert = VULKAN_NONLIT_2D_VERT;
        shader_resources.nonlit_2d_vert_size = sizeof(VULKAN_NONLIT_2D_VERT);
        shader_resources.nonlit_2d_frag = VULKAN_NONLIT_2D_FRAG;
        shader_resources.nonlit_2d_frag_size = sizeof(VULKAN_NONLIT_2D_FRAG);

        shader_resources.world_3d_vert = VULKAN_WORLD_3D_VERT;
        shader_resources.world_3d_vert_size = sizeof(VULKAN_WORLD_3D_VERT);
        shader_resources.world_3d_frag = VULKAN_WORLD_3D_FRAG;
        shader_resources.world_3d_frag_size = sizeof(VULKAN_WORLD_3D_FRAG);

        shader_resources.quad_vert = VULKAN_QUAD_VERT;
        shader_resources.quad_vert_size = sizeof(VULKAN_QUAD_VERT);
        shader_resources.quad_frag = VULKAN_QUAD_FRAG;
        shader_resources.quad_frag_size = sizeof(VULKAN_QUAD_FRAG);

        shader_resources.sprite_vert = VULKAN_SPRITE_VERT;
        shader_resources.sprite_vert_size = sizeof(VULKAN_SPRITE_VERT);
        shader_resources.sprite_frag = VULKAN_SPRITE_FRAG;
        shader_resources.sprite_frag_size = sizeof(VULKAN_SPRITE_FRAG);

        shader_resources.text_vert = VULKAN_TEXT_VERT;
        shader_resources.text_vert_size = sizeof(VULKAN_TEXT_VERT);
        shader_resources.text_frag = VULKAN_TEXT_FRAG;
        shader_resources.text_frag_size = sizeof(VULKAN_TEXT_FRAG);

        shader_resources.mesh_vert = VULKAN_MESH_VERT;
        shader_resources.mesh_vert_size = sizeof(VULKAN_MESH_VERT);
        shader_resources.mesh_frag = VULKAN_MESH_FRAG;
        shader_resources.mesh_frag_size = sizeof(VULKAN_MESH_FRAG);
        break;
    }

    graphics_data.shader_resources = shader_resources;

    uint16_t indices[6] = {
        0, 1, 3, 1, 2, 3
    };

    graphics_data.index_buffer =
        mg_rhi_renderer_create_index_buffer(sizeof(indices), indices);

#pragma region SCREEN_DATA
    {
        const float vertices[16] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 1.0f
        };

        graphics_data.screen_vertex_buffer =
            mg_rhi_renderer_create_vertex_buffer(sizeof(vertices), vertices);

        mg_sampler_create_info_t sampler_create_info = {
            .mag_filter = MG_SAMPLER_FILTER_NEAREST,
            .min_filter = MG_SAMPLER_FILTER_NEAREST,
            .address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
        };
        
        graphics_data.screen_sampler =
            mg_rhi_renderer_create_sampler(&sampler_create_info);
    }
#pragma endregion

#pragma region QUAD_DATA
    {
        mg_shader_source_t vertex_shader = {
            .code = shader_resources.quad_vert,
            .code_size = shader_resources.quad_vert_size
        };

        mg_shader_source_t fragment_shader = {
            .code = shader_resources.quad_frag,
            .code_size = shader_resources.quad_frag_size
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
            .vertex_shader = vertex_shader,
            .fragment_shader = fragment_shader,

            vertex_layout = vertex_layout,

            .polygon_mode = MG_POLYGON_MODE_FILL,
            .cull_mode = MG_CULL_MODE_NONE,
            .front_face = MG_FRONT_FACE_CW,

            .color_blend = {
                .blend_enabled = true,
                .src_color_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
                .dst_color_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = MG_BLEND_OP_ADD,
                .src_alpha_blend_factor = MG_BLEND_FACTOR_ONE,
                .dst_alpha_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
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
#pragma endregion

#pragma region SPRITE_DATA
    {
        mg_shader_source_t vertex_shader = {
            .code = shader_resources.sprite_vert,
            .code_size = shader_resources.sprite_vert_size
        };

        mg_shader_source_t fragment_shader = {
            .code = shader_resources.sprite_frag,
            .code_size = shader_resources.sprite_frag_size
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
            .vertex_shader = vertex_shader,
            .fragment_shader = fragment_shader,

            vertex_layout = vertex_layout,

            .polygon_mode = MG_POLYGON_MODE_FILL,
            .cull_mode = MG_CULL_MODE_NONE,
            .front_face = MG_FRONT_FACE_CW,

            .color_blend = {
                .blend_enabled = true,
                .src_color_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
                .dst_color_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = MG_BLEND_OP_ADD,
                .src_alpha_blend_factor = MG_BLEND_FACTOR_ONE,
                .dst_alpha_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
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
#pragma endregion

#pragma region TEXT_DATA
    {
        mg_shader_source_t vertex_shader = {
            .code = shader_resources.text_vert,
            .code_size = shader_resources.text_vert_size
        };

        mg_shader_source_t fragment_shader = {
            .code = shader_resources.text_frag,
            .code_size = shader_resources.text_frag_size
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
            .vertex_shader = vertex_shader,
            .fragment_shader = fragment_shader,

            vertex_layout = vertex_layout,

            .polygon_mode = MG_POLYGON_MODE_FILL,
            .cull_mode = MG_CULL_MODE_NONE,
            .front_face = MG_FRONT_FACE_CW,

            .color_blend = {
                .blend_enabled = true,
                .src_color_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
                .dst_color_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = MG_BLEND_OP_ADD,
                .src_alpha_blend_factor = MG_BLEND_FACTOR_ONE,
                .dst_alpha_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .alpha_blend_op = MG_BLEND_OP_ADD
            },

            .has_push_constants = true,
            .push_constants_size = sizeof(graphics_data.text_data.push_constant)
        };

        graphics_data.text_data.pipeline =
            mg_rhi_renderer_create_pipeline(&pipeline_create_info);

        for (uint32_t i = 0; i < MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS; i++)
        {
            graphics_data.text_data.vertex_buffers[i] =
                mg_rhi_renderer_create_dynamic_vertex_buffer(MG_CONFIG_GRAPHICS_MAX_TEXT_LENGTH * 16 * sizeof(float));

            graphics_data.text_data.index_buffers[i] =
                mg_rhi_renderer_create_dynamic_index_buffer(MG_CONFIG_GRAPHICS_MAX_TEXT_LENGTH * 6 * sizeof(float));
        }
    }
#pragma endregion

#pragma region MESH_DATA
    {
        mg_shader_source_t vertex_shader = {
            .code = shader_resources.mesh_vert,
            .code_size = shader_resources.mesh_vert_size
        };

        mg_shader_source_t fragment_shader = {
            .code = shader_resources.mesh_frag,
            .code_size = shader_resources.mesh_frag_size
        };

        mg_vertex_attribute_info_t vertex_attributes[3] = {
            {
                .location = 0,
                .offset = 0,
                .format = MG_VERTEX_FORMAT_FLOAT3
            },
            {
                .location = 1,
                .offset = 3 * sizeof(float),
                .format = MG_VERTEX_FORMAT_FLOAT3
            },
            {
                .location = 2,
                .offset = 6 * sizeof(float),
                .format = MG_VERTEX_FORMAT_FLOAT2
            }
        };

        mg_vertex_layout_info_t vertex_layout = {
            .stride = 8 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 3
        };

        mg_pipeline_create_info_t pipeline_create_info = {
            .vertex_shader = vertex_shader,
            .fragment_shader = fragment_shader,

            vertex_layout = vertex_layout,

            .polygon_mode = MG_POLYGON_MODE_FILL,
            .cull_mode = MG_CULL_MODE_NONE,
            .front_face = MG_FRONT_FACE_CW,

            .color_blend = {
                .blend_enabled = true,
                .src_color_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
                .dst_color_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = MG_BLEND_OP_ADD,
                .src_alpha_blend_factor = MG_BLEND_FACTOR_ONE,
                .dst_alpha_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .alpha_blend_op = MG_BLEND_OP_ADD
            },

            .depth_stencil = {
                .depth_compare_op = MG_COMPARE_OP_LESS,
                .depth_test_enable = true,
                .depth_write_enable = true
            },

            .has_push_constants = true,
            .push_constants_size = sizeof(graphics_data.mesh_data.push_constant)
        };

        graphics_data.mesh_data.pipeline =
            mg_rhi_renderer_create_pipeline(&pipeline_create_info);
    }
#pragma endregion
}

void mg_graphics_shutdown(void)
{
    mg_rhi_renderer_wait();

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

    // Text Data
    {
        for (uint32_t i = 0; i < MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS; i++)
        {
            mg_rhi_renderer_destroy_dynamic_vertex_buffer(graphics_data.text_data.vertex_buffers[i]);
            mg_rhi_renderer_destroy_dynamic_index_buffer(graphics_data.text_data.index_buffers[i]);
        }

        mg_rhi_renderer_destroy_pipeline(graphics_data.text_data.pipeline);
    }

    // Mesh Data
    {
        mg_rhi_renderer_destroy_pipeline(graphics_data.mesh_data.pipeline);
    }

    mg_rhi_renderer_destroy_sampler(graphics_data.screen_sampler);
    mg_rhi_renderer_destroy_vertex_buffer(graphics_data.screen_vertex_buffer);
    mg_rhi_renderer_destroy_index_buffer(graphics_data.index_buffer);
}

void mg_graphics_resize(int32_t width, int32_t height)
{
    graphics_data.width = width;
    graphics_data.height = height;

    graphics_data.world_2d_ubo.resolution = (mg_vec2_t){ width, height };

    for (uint32_t i = 0; i < graphics_data.layer_count; i++)
        mg_graphics_resize_render_layer(graphics_data.layers[i]);
}

void mg_graphics_begin(void)
{
    graphics_data.frame_data.pipeline_bind_id = 0;
    graphics_data.frame_data.vertex_buffer_bind_id = 0;
    graphics_data.frame_data.index_buffer_bind_id = 0;
    graphics_data.frame_data.text_index = 0;
    graphics_data.world_2d_ubo.light_count = 0;
}

void mg_graphics_end(void)
{
    mg_render_pass_begin_info_t render_pass_begin_info = {
        .render_area = (mg_vec4_t) {0.0f, 0.0f, graphics_data.width, graphics_data.height},
        .clear_value = (mg_vec4_t) {0.0f, 0.0f, 0.0f, 1.0f}
    };

    mg_rhi_renderer_begin_default_render_pass(&render_pass_begin_info);
    mg_rhi_renderer_viewport(0, 0, graphics_data.width, graphics_data.height);

    mg_rhi_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT16);

    for (uint32_t i = 0; i < graphics_data.layer_count; i++)
    {
        mg_render_layer_t *layer = graphics_data.layers[i];
        mg_rhi_renderer_bind_pipeline(layer->pipeline);
        if (layer->internal_ub.internal_data)
            mg_rhi_renderer_bind_uniform_buffer(layer->internal_ub, layer->pipeline);
        mg_rhi_renderer_bind_image(layer->color_attachment, layer->pipeline);
        mg_rhi_renderer_bind_vertex_buffer(graphics_data.screen_vertex_buffer);
        mg_rhi_renderer_draw_indexed(6, 0);
    }

    mg_rhi_renderer_end_render_pass();
}

void mg_graphics_2d_begin_background(mg_render_layer_t *layer, mg_vec4_t clear_color)
{
    mg_graphics_begin_render_layer(layer, clear_color);

    const int32_t width = graphics_data.width;
    const int32_t height = graphics_data.height;

    graphics_data.global_ubo.view_projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);

    mg_rhi_renderer_update_uniform_buffer(layer->global_ub,
        sizeof(graphics_data.global_ubo), 0, &graphics_data.global_ubo);

    mg_rhi_renderer_bind_uniform_buffer(layer->global_ub, layer->pipeline);
}

void mg_graphics_2d_end_background(mg_render_layer_t *layer)
{
    mg_graphics_end_render_layer(layer);
}

void mg_graphics_2d_begin_world(mg_render_layer_t *layer, mg_world_info_2d_t *world_info)
{
    mg_graphics_begin_render_layer(layer, (mg_vec4_t) {0.0f, 0.0f, 0.0f, 0.0f});

    const int32_t width = graphics_data.width;
    const int32_t height = graphics_data.height;

    mg_mat4_t projection;

    projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);
    projection = mg_mat4_translate(projection, (mg_vec3_t) { -world_info->camera_position.x / width * 2.0f, -world_info->camera_position.y / height * 2.0f, 0.0f });
    projection = mg_mat4_scale(projection, (mg_vec3_t) { world_info->camera_zoom.x, world_info->camera_zoom.y, 1.0f });

    graphics_data.world_2d_ubo.flag_enable_vignette = world_info->flags & MG_WORLD_FLAG_2D_ENABLE_VIGNETTE;
    graphics_data.world_2d_ubo.flag_enable_shadows = world_info->flags & MG_WORLD_FLAG_2D_ENABLE_SHADOWS;
    graphics_data.world_2d_ubo.flag_enable_lights = world_info->flags & MG_WORLD_FLAG_2D_ENABLE_LIGHTS;
    graphics_data.world_2d_ubo.shadow_data = (mg_vec4_t) {world_info->shadow_offset.x, world_info->shadow_offset.y, world_info->shadown_opacity, 0.0f};

    graphics_data.world_2d_ubo.global_light_data =
        (mg_vec4_t) { world_info->global_light_color.x, world_info->global_light_color.y, world_info->global_light_color.z,
        world_info->global_light_intensity};

    graphics_data.global_ubo.view_projection = projection;

    mg_rhi_renderer_update_uniform_buffer(layer->global_ub,
        sizeof(graphics_data.global_ubo), 0, &graphics_data.global_ubo);

    mg_rhi_renderer_bind_uniform_buffer(layer->global_ub, layer->pipeline);
}

void mg_graphics_2d_end_world(mg_render_layer_t *layer)
{
    mg_graphics_end_render_layer(layer);

    mg_rhi_renderer_update_uniform_buffer(layer->internal_ub,
        sizeof(graphics_data.world_2d_ubo), 0, &graphics_data.world_2d_ubo);
}

void mg_graphics_2d_begin_nonlit(mg_render_layer_t *layer)
{
    mg_graphics_begin_render_layer(layer, (mg_vec4_t) {0.0f, 0.0f, 0.0f, 0.0f});

    const int32_t width = graphics_data.width;
    const int32_t height = graphics_data.height;

    graphics_data.global_ubo.view_projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);

    mg_rhi_renderer_update_uniform_buffer(layer->global_ub,
        sizeof(graphics_data.global_ubo), 0, &graphics_data.global_ubo);

    mg_rhi_renderer_bind_uniform_buffer(layer->global_ub, layer->pipeline);
}

void mg_graphics_2d_end_nonlit(mg_render_layer_t *layer)
{
    mg_graphics_end_render_layer(layer);
}

void mg_graphics_2d_draw_rect(mg_mat4_t model, mg_vec4_t color)
{
    if (graphics_data.frame_data.pipeline_bind_id != 1)
    {
        mg_rhi_renderer_bind_pipeline(graphics_data.quad_data.pipeline);
        graphics_data.frame_data.pipeline_bind_id = 1;
    }

    if (graphics_data.frame_data.vertex_buffer_bind_id != 1)
    {
        mg_rhi_renderer_bind_vertex_buffer(graphics_data.quad_data.vertex_buffer);
        graphics_data.frame_data.vertex_buffer_bind_id = 1;
    }

    if (graphics_data.frame_data.index_buffer_bind_id != 1)
    {
        mg_rhi_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT16);
        graphics_data.frame_data.index_buffer_bind_id = 1;
    }

    graphics_data.quad_data.push_constant.model = model;
    graphics_data.quad_data.push_constant.color = color;

    mg_rhi_renderer_push_constants(graphics_data.quad_data.pipeline,
        sizeof(graphics_data.quad_data.push_constant), &graphics_data.quad_data.push_constant);

    mg_rhi_renderer_draw_indexed(6, 0);
}

void mg_graphics_2d_draw_sprite(mg_mat4_t model, mg_vec4_t color, mg_sprite_t *sprite)
{
    if (graphics_data.frame_data.pipeline_bind_id != 2)
    {
        mg_rhi_renderer_bind_pipeline(graphics_data.sprite_data.pipeline);
        graphics_data.frame_data.pipeline_bind_id = 2;
    }

    if (graphics_data.frame_data.vertex_buffer_bind_id != 2)
    {
        mg_rhi_renderer_bind_vertex_buffer(graphics_data.sprite_data.vertex_buffer);
        graphics_data.frame_data.vertex_buffer_bind_id = 2;
    }

    if (graphics_data.frame_data.index_buffer_bind_id != 2)
    {
        mg_rhi_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT16);
        graphics_data.frame_data.index_buffer_bind_id = 2;
    }

    mg_rhi_renderer_bind_image(sprite->texture->image, graphics_data.sprite_data.pipeline);

    graphics_data.sprite_data.push_constant.model = model;
    graphics_data.sprite_data.push_constant.color = color;
    graphics_data.sprite_data.push_constant.rect =
        (mg_vec4_t) { sprite->x, sprite->y, sprite->width, sprite->height };

    mg_rhi_renderer_push_constants(graphics_data.sprite_data.pipeline,
        sizeof(graphics_data.sprite_data.push_constant), &graphics_data.sprite_data.push_constant);

    mg_rhi_renderer_draw_indexed(6, 0);
}

void mg_graphics_2d_draw_line(mg_vec2_t point1, mg_vec2_t point2, float width, mg_vec4_t color)
{
    mg_mat4_t model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3_t) { 0.5f, 0.0f, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3_t) { mg_vec2_distance(point1, point2), width, 1.0f });
    model = mg_mat4_rotate_z(model, atan2f(point2.y - point1.y, point2.x - point1.x));
    model = mg_mat4_translate(model, (mg_vec3_t) { point1.x, point1.y, 0.0f });
    mg_graphics_2d_draw_rect(model, color);
}

void mg_graphics_2d_draw_text(mg_mat4_t model, mg_vec4_t color, const char *text, mg_font_t *font)
{
    if (graphics_data.frame_data.text_index >= MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS)
        return;

    if (graphics_data.frame_data.pipeline_bind_id != 3)
    {
        mg_rhi_renderer_bind_pipeline(graphics_data.text_data.pipeline);
        graphics_data.frame_data.pipeline_bind_id = 3;
        graphics_data.frame_data.vertex_buffer_bind_id = 3;
        graphics_data.frame_data.index_buffer_bind_id = 3;
    }

    mg_rhi_renderer_bind_dynamic_vertex_buffer(graphics_data.text_data.vertex_buffers[graphics_data.frame_data.text_index]);
    mg_rhi_renderer_bind_dynamic_index_buffer(graphics_data.text_data.index_buffers[graphics_data.frame_data.text_index], MG_INDEX_TYPE_UINT32);

    mg_rhi_renderer_bind_image(font->texture->image, graphics_data.text_data.pipeline);

    graphics_data.text_data.push_constant.model = model;
    graphics_data.text_data.push_constant.color = color;

    mg_rhi_renderer_push_constants(graphics_data.text_data.pipeline,
        sizeof(graphics_data.text_data.push_constant), &graphics_data.text_data.push_constant);

    float x = 0;
    float y = 0;

    uint32_t i;
    for (i = 0; i < strlen(text); i++)
    {
        switch (text[i])
        {
        case '\n':
            x = 0.0f;
            y += 1.0f;

            graphics_data.text_data.vertices[i * 16] = 0;
            graphics_data.text_data.vertices[i * 16 + 1] = 0;
            graphics_data.text_data.vertices[i * 16 + 2] = 0;
            graphics_data.text_data.vertices[i * 16 + 3] = 0;

            graphics_data.text_data.vertices[i * 16 + 4] = 0;
            graphics_data.text_data.vertices[i * 16 + 5] = 0;
            graphics_data.text_data.vertices[i * 16 + 6] = 0;
            graphics_data.text_data.vertices[i * 16 + 7] = 0;

            graphics_data.text_data.vertices[i * 16 + 8] = 0;
            graphics_data.text_data.vertices[i * 16 + 9] = 0;
            graphics_data.text_data.vertices[i * 16 + 10] = 0;
            graphics_data.text_data.vertices[i * 16 + 11] = 0;

            graphics_data.text_data.vertices[i * 16 + 12] = 0;
            graphics_data.text_data.vertices[i * 16 + 13] = 0;
            graphics_data.text_data.vertices[i * 16 + 14] = 0;
            graphics_data.text_data.vertices[i * 16 + 15] = 0;
        break;
        case ' ':
            x += 0.5f;
            graphics_data.text_data.vertices[i * 16] = 0;
            graphics_data.text_data.vertices[i * 16 + 1] = 0;
            graphics_data.text_data.vertices[i * 16 + 2] = 0;
            graphics_data.text_data.vertices[i * 16 + 3] = 0;

            graphics_data.text_data.vertices[i * 16 + 4] = 0;
            graphics_data.text_data.vertices[i * 16 + 5] = 0;
            graphics_data.text_data.vertices[i * 16 + 6] = 0;
            graphics_data.text_data.vertices[i * 16 + 7] = 0;

            graphics_data.text_data.vertices[i * 16 + 8] = 0;
            graphics_data.text_data.vertices[i * 16 + 9] = 0;
            graphics_data.text_data.vertices[i * 16 + 10] = 0;
            graphics_data.text_data.vertices[i * 16 + 11] = 0;

            graphics_data.text_data.vertices[i * 16 + 12] = 0;
            graphics_data.text_data.vertices[i * 16 + 13] = 0;
            graphics_data.text_data.vertices[i * 16 + 14] = 0;
            graphics_data.text_data.vertices[i * 16 + 15] = 0;
            break;
        default:
            {
                float _x, _y;
                stbtt_bakedchar b = font->cdata[(int32_t)text[i]];
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(font->cdata, 1024, 1024, (int32_t)text[i] - 32, &_x, &_y, &q, 0);

                float descent = q.y1 / MG_CONFIG_GRAPHICS_FONT_HEIGHT;

                graphics_data.text_data.vertices[i * 16] = x;
                graphics_data.text_data.vertices[i * 16 + 1] = y - (q.y1 - q.y0) / MG_CONFIG_GRAPHICS_FONT_HEIGHT + descent;
                graphics_data.text_data.vertices[i * 16 + 2] = q.s0 * 0.5f;
                graphics_data.text_data.vertices[i * 16 + 3] = q.t0;

                graphics_data.text_data.vertices[i * 16 + 4] = x + (q.x1 - q.x0) / MG_CONFIG_GRAPHICS_FONT_HEIGHT;
                graphics_data.text_data.vertices[i * 16 + 5] = y - (q.y1 - q.y0) / MG_CONFIG_GRAPHICS_FONT_HEIGHT + descent;
                graphics_data.text_data.vertices[i * 16 + 6] = q.s1 * 0.5f - 0.002f;
                graphics_data.text_data.vertices[i * 16 + 7] = q.t0;

                graphics_data.text_data.vertices[i * 16 + 8] = x + (q.x1 - q.x0) / MG_CONFIG_GRAPHICS_FONT_HEIGHT;
                graphics_data.text_data.vertices[i * 16 + 9] = y + descent;
                graphics_data.text_data.vertices[i * 16 + 10] = q.s1 * 0.5f - 0.002f;
                graphics_data.text_data.vertices[i * 16 + 11] = q.t1;

                graphics_data.text_data.vertices[i * 16 + 12] = x;
                graphics_data.text_data.vertices[i * 16 + 13] = y + descent;
                graphics_data.text_data.vertices[i * 16 + 14] = q.s0 * 0.5f;
                graphics_data.text_data.vertices[i * 16 + 15] = q.t1;

                graphics_data.text_data.indices[i * 6] = i * 4;
                graphics_data.text_data.indices[i * 6 + 1] = 1 + i * 4;
                graphics_data.text_data.indices[i * 6 + 2] = 3 + i * 4;
                graphics_data.text_data.indices[i * 6 + 3] = 1 + i * 4;
                graphics_data.text_data.indices[i * 6 + 4] = 2 + i * 4;
                graphics_data.text_data.indices[i * 6 + 5] = 3 + i * 4;

                x += (q.x1 - q.x0 + 9.0f) / MG_CONFIG_GRAPHICS_FONT_HEIGHT;
            }
        }
    }

    mg_rhi_renderer_update_dynamic_vertex_buffer(graphics_data.text_data.vertex_buffers[graphics_data.frame_data.text_index], sizeof(graphics_data.text_data.vertices), graphics_data.text_data.vertices);
    mg_rhi_renderer_update_dynamic_index_buffer(graphics_data.text_data.index_buffers[graphics_data.frame_data.text_index], sizeof(graphics_data.text_data.indices), graphics_data.text_data.indices);

    mg_rhi_renderer_draw_indexed(i * 6, 0);

    graphics_data.frame_data.text_index++;
}

// TODO (box): Fix this
/*void mg_graphics_draw_reflective_rect(mg_vec2_t position, mg_vec2_t scale, mg_vec4_t color)
{
    if (graphics_data.frame_data.bind_id != 3)
    {
        mg_rhi_renderer_bind_pipeline(graphics_data.sprite_data.pipeline);
        mg_rhi_renderer_bind_vertex_buffer(graphics_data.sprite_data.vertex_buffer);
        graphics_data.frame_data.bind_id = 3;
    }

    mg_rhi_renderer_bind_image(graphics_data.world_data.image, graphics_data.sprite_data.pipeline);

    mg_mat4_t model = mg_mat4_identity();
    model = mg_mat4_scale(model, (mg_vec3_t){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3_t){ position.x, position.y, 0.0f });

    graphics_data.sprite_data.push_constant.model = model;
    graphics_data.sprite_data.push_constant.color = color;
    graphics_data.sprite_data.push_constant.rect =
        (mg_vec4_t) { 0, 0, 0.3f, 0.3f };

    mg_rhi_renderer_push_constants(graphics_data.sprite_data.pipeline,
        sizeof(graphics_data.sprite_data.push_constant), &graphics_data.sprite_data.push_constant);

    mg_rhi_renderer_draw_indexed(6, 0);
}*/

void mg_graphics_2d_draw_point_light(mg_vec2_t position, float scale, float intensity, mg_vec3_t color)
{
    if (graphics_data.world_2d_ubo.light_count >= MG_GRAPHICS_2D_MAX_LIGHT_COUNT)
        graphics_data.world_2d_ubo.light_count = 0;

    graphics_data.world_2d_ubo.light_transform[graphics_data.world_2d_ubo.light_count] =
        (mg_vec4_t) { position.x, position.y, scale, intensity };
    graphics_data.world_2d_ubo.light_colors[graphics_data.world_2d_ubo.light_count] =
        (mg_vec4_t) { color.r, color.g, color.b, 1.0f };

    graphics_data.world_2d_ubo.light_count++;
}

void mg_graphics_3d_begin_world(mg_render_layer_t *layer)
{
    mg_graphics_begin_render_layer(layer, (mg_vec4_t) {0.0f, 0.0f, 0.0f, 0.0f});

    graphics_data.global_ubo.view_projection = mg_mat4_perspective(45 * MG_DEG2RAD, (float)graphics_data.width / (float)graphics_data.height, 0.001f, 10.0f);
    graphics_data.global_ubo.view_projection = mg_mat4_multiply(mg_mat4_look_at(mg_vec3(0.0f, 0.0f, 1.0f), mg_vec3(0.0f, 0.0f, 3.0f), mg_vec3(0.0f, 1.0f, 0.0f)), graphics_data.global_ubo.view_projection);

    mg_rhi_renderer_update_uniform_buffer(layer->global_ub,
        sizeof(graphics_data.global_ubo), 0, &graphics_data.global_ubo);

    mg_rhi_renderer_bind_uniform_buffer(layer->global_ub, layer->pipeline);
}

void mg_graphics_3d_end_world(mg_render_layer_t *layer)
{
    mg_rhi_renderer_end_render_pass();
}

void mg_graphics_3d_draw_mesh(mg_mat4_t model, mg_vec4_t color, mg_mesh_t* mesh, mg_texture_t* texture)
{
    if (graphics_data.frame_data.pipeline_bind_id != 4)
    {
        mg_rhi_renderer_bind_pipeline(graphics_data.mesh_data.pipeline);
        graphics_data.frame_data.pipeline_bind_id = 4;
        graphics_data.frame_data.vertex_buffer_bind_id = 4;
        graphics_data.frame_data.index_buffer_bind_id = 4;
    }

    mg_rhi_renderer_bind_vertex_buffer(mesh->vertex_buffer);
    mg_rhi_renderer_bind_index_buffer(mesh->index_buffer, MG_INDEX_TYPE_UINT32);

    mg_rhi_renderer_bind_image(texture->image, graphics_data.mesh_data.pipeline);

    graphics_data.mesh_data.push_constant.model = model;
    graphics_data.mesh_data.push_constant.color = color;

    mg_rhi_renderer_push_constants(graphics_data.mesh_data.pipeline,
        sizeof(graphics_data.mesh_data.push_constant), &graphics_data.mesh_data.push_constant);

    mg_rhi_renderer_draw_indexed(mesh->index_count, 0);
}