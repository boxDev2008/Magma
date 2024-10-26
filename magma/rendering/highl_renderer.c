#include "highl_renderer.h"

#include "math/mat4.h"
#include "math/math.h"

#include "shaders/vulkan/2d/world/vert.spv.h"
#include "shaders/vulkan/2d/world/frag.spv.h"

#include "shaders/vulkan/2d/nonlit/vert.spv.h"
#include "shaders/vulkan/2d/nonlit/frag.spv.h"

#include "shaders/vulkan/2d/quad/vert.spv.h"
#include "shaders/vulkan/2d/quad/frag.spv.h"

#include "shaders/vulkan/2d/sprite/vert.spv.h"
#include "shaders/vulkan/2d/sprite/frag.spv.h"

#include "shaders/vulkan/2d/text/vert.spv.h"
#include "shaders/vulkan/2d/text/frag.spv.h"

#include "shaders/vulkan/3d/world/vert.spv.h"
#include "shaders/vulkan/3d/world/frag.spv.h"

#include "shaders/vulkan/3d/mesh/vert.spv.h"
#include "shaders/vulkan/3d/mesh/frag.spv.h"

#include "shaders/opengl/2d/world/vert.h"
#include "shaders/opengl/2d/world/frag.h"

#include "shaders/opengl/2d/nonlit/vert.h"
#include "shaders/opengl/2d/nonlit/frag.h"

#include "shaders/opengl/2d/quad/vert.h"
#include "shaders/opengl/2d/quad/frag.h"

#include "shaders/opengl/2d/sprite/vert.h"
#include "shaders/opengl/2d/sprite/frag.h"

#include "shaders/opengl/2d/text/vert.h"
#include "shaders/opengl/2d/text/frag.h"

#include "shaders/opengl/3d/world/vert.h"
#include "shaders/opengl/3d/world/frag.h"

#include "shaders/opengl/3d/mesh/vert.h"
#include "shaders/opengl/3d/mesh/frag.h"

#if MG_PLATFORM_WINDOWS
	#include "shaders/d3d11/2d/world/vert.h"
	#include "shaders/d3d11/2d/world/frag.h"

	#include "shaders/d3d11/2d/nonlit/vert.h"
	#include "shaders/d3d11/2d/nonlit/frag.h"

	#include "shaders/d3d11/2d/quad/vert.h"
	#include "shaders/d3d11/2d/quad/frag.h"

	#include "shaders/d3d11/2d/sprite/vert.h"
	#include "shaders/d3d11/2d/sprite/frag.h"

	#include "shaders/d3d11/2d/text/vert.h"
	#include "shaders/d3d11/2d/text/frag.h"

	#include "shaders/d3d11/3d/world/vert.h"
	#include "shaders/d3d11/3d/world/frag.h"

	#include "shaders/d3d11/3d/mesh/vert.h"
	#include "shaders/d3d11/3d/mesh/frag.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MG_HIGHL_RENDERER_2D_MAX_LIGHT_COUNT 1024

#define MG_HIGHL_RENDERER_2D_GLOBAL_UNIFORM_BUFFER_WORLD_INDEX 0
#define MG_HIGHL_RENDERER_2D_GLOBAL_UNIFORM_BUFFER_NONLIT_INDEX 1

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

typedef struct mg_highl_renderer_data
{
    mg_index_buffer index_buffer;
    
    mg_vertex_buffer screen_vertex_buffer;
    mg_sampler screen_sampler;
    
    mg_render_layer *layers[MG_CONFIG_GRAPHICS_MAX_RENDER_LAYERS];
    uint32_t layer_count;
    
    struct
    {
        mg_mat4 view_projection;
    }
    global_ubo;
    
    struct
    {
        mg_vec4 light_transform[MG_HIGHL_RENDERER_2D_MAX_LIGHT_COUNT];
        mg_vec4 light_colors[MG_HIGHL_RENDERER_2D_MAX_LIGHT_COUNT];
        mg_vec4 global_light_data;
        mg_vec4 shadow_data;
        mg_vec2 resolution;
        int32_t light_count;
        int32_t flag_enable_vignette;
        int32_t flag_enable_lights;
        int32_t flag_enable_shadows;
    }
    world_ubo_2d;
    
    struct
    {
        mg_pipeline pipeline;
        mg_vertex_buffer vertex_buffer;
        
        struct
        {
            mg_mat4 model;
            mg_vec4 color;
        }
        push_constant;
    }
    quad_data;
    
    struct
    {
        mg_pipeline pipeline;
        mg_vertex_buffer vertex_buffer;
        
        struct
        {
            mg_mat4 model;
            mg_vec4 color;
            mg_vec4 rect;
        }
        push_constant;
    }
    sprite_data;
    
    struct
    {
        mg_pipeline pipeline;
        
        mg_dynamic_vertex_buffer vertex_buffers[MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS];
        mg_dynamic_index_buffer index_buffers[MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS];
        
        float vertices[MG_CONFIG_HIGHL_RENDERER_MAX_TEXT_LENGTH * 16];
        uint16_t indices[MG_CONFIG_HIGHL_RENDERER_MAX_TEXT_LENGTH * 6];
        
        struct
        {
            mg_mat4 model;
            mg_vec4 color;
        }
        push_constant;
    }
    text_data;
    
    struct
    {
        mg_pipeline pipeline;
        
        struct
        {
            mg_mat4 model;
            mg_vec4 color;
        }
        push_constant;
    }
    mesh_data_2d;

    struct
    {
        mg_pipeline pipeline;
        
        struct
        {
            mg_mat4 model;
            mg_vec4 color;
        }
        push_constant;
    }
    mesh_data_3d;
    
    struct
    {
        uint16_t bind_id;
        uint32_t text_index;
    }
    frame_data;
    
    mg_shader_resources_t shader_resources;
    mg_render_layer *current_layer;
    int32_t width, height;
}
mg_highl_renderer_data_t;

static mg_highl_renderer_data_t graphics_data;

mg_render_layer *mg_highl_renderer_create_render_layer(mg_shader_source vertex_shader, mg_shader_source fragment_shader,
	bool has_depth_stencil_attachment, size_t global_ub_size, size_t internal_ub_size)
{
    mg_render_layer *layer = (mg_render_layer*)calloc(1, sizeof(mg_render_layer));
    mg_render_pass_create_info render_pass_info = {
        .color_attachment = {
            .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB
        }
    };
    
    mg_image_create_info color_attachment_info = {
        .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
        .type = MG_IMAGE_TYPE_2D,
        .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
        .width = graphics_data.width,
        .height = graphics_data.height
    };
    
    layer->color_attachment =
        mg_lowl_renderer_create_image(&color_attachment_info);

    layer->image_array = mg_lowl_renderer_create_image_array();
    mg_lowl_renderer_update_image_array(layer->image_array, &layer->color_attachment, &graphics_data.screen_sampler, 1);

    mg_framebuffer_create_info framebuffer_info = {
        .width = graphics_data.width,
        .height = graphics_data.height,
        .color_attachment = layer->color_attachment
    };
    
    if (has_depth_stencil_attachment)
    {
        render_pass_info.depth_stencil_attachment = (mg_attachment_info){
            .format = MG_PIXEL_FORMAT_D32_SFLOAT
        };
        render_pass_info.has_depth_stencil_attachment = true;
        
        mg_image_create_info depth_stencil_attachment_create_info = {
            .format = MG_PIXEL_FORMAT_D32_SFLOAT,
            .type = MG_IMAGE_TYPE_2D,
            .usage = MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT,
            .width = graphics_data.width,
            .height = graphics_data.height
        };
        
        framebuffer_info.depth_stencil_attachment = layer->depth_stencil_attachment =
            mg_lowl_renderer_create_image(&depth_stencil_attachment_create_info);
    }
    
    layer->render_pass =
        mg_lowl_renderer_create_render_pass(&render_pass_info);
    
    framebuffer_info.render_pass = layer->render_pass;
    
    layer->framebuffer =
        mg_lowl_renderer_create_framebuffer(&framebuffer_info);
    
    const mg_vertex_attribute_info vertex_attributes[2] = {
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
    
    mg_vertex_layout_info vertex_layout = {
        .stride = 4 * sizeof(float),
        .attributes = vertex_attributes,
        .attribute_count = 2
    };
    
    mg_pipeline_create_info pipeline_create_info = {
        .shader = {
            .vertex = vertex_shader,
            .fragment = fragment_shader, 
        },
        
        vertex_layout = vertex_layout,

        .primitive_topology = MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
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
        mg_lowl_renderer_create_pipeline(&pipeline_create_info);
    
    layer->global_ub = mg_lowl_renderer_create_uniform_buffer(global_ub_size);
    
    if (internal_ub_size > 0)
        layer->internal_ub = mg_lowl_renderer_create_uniform_buffer(internal_ub_size);
    
    return layer;
}

void mg_highl_renderer_destroy_render_layer(mg_render_layer *layer)
{
    mg_lowl_renderer_destroy_framebuffer(layer->framebuffer);
    mg_lowl_renderer_destroy_image(layer->color_attachment);

    mg_lowl_renderer_destroy_image_array(layer->image_array);
    
    if (layer->depth_stencil_attachment)
        mg_lowl_renderer_destroy_image(layer->depth_stencil_attachment);
    
    mg_lowl_renderer_destroy_pipeline(layer->pipeline);
    mg_lowl_renderer_destroy_render_pass(layer->render_pass);
    
    mg_lowl_renderer_destroy_uniform_buffer(layer->global_ub);
    
    if (layer->internal_ub)
        mg_lowl_renderer_destroy_uniform_buffer(layer->internal_ub);
    
    free(layer);
}

void mg_highl_renderer_resize_render_layer(mg_render_layer *layer, int32_t width, int32_t height)
{
    mg_lowl_renderer_destroy_framebuffer(layer->framebuffer);
    mg_lowl_renderer_destroy_image(layer->color_attachment);

    mg_image_create_info color_attachment_info = {
        .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
        .type = MG_IMAGE_TYPE_2D,
        .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
        .width = width,
        .height = height
    };
    
    layer->color_attachment =
        mg_lowl_renderer_create_image(&color_attachment_info);

    mg_lowl_renderer_update_image_array(layer->image_array, &layer->color_attachment, &graphics_data.screen_sampler, 1);

    mg_framebuffer_create_info framebuffer_create_info = {
        .width = width,
        .height = height,
        .render_pass = layer->render_pass,
        .color_attachment = layer->color_attachment
    };
    
    if (layer->depth_stencil_attachment)
    {
        mg_lowl_renderer_destroy_image(layer->depth_stencil_attachment);

        mg_image_create_info depth_stencil_attachment_create_info = {
            .format = MG_PIXEL_FORMAT_D32_SFLOAT,
            .type = MG_IMAGE_TYPE_2D,
            .usage = MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT,
            .width = width,
            .height = height
        };
        
        layer->depth_stencil_attachment =
            mg_lowl_renderer_create_image(&depth_stencil_attachment_create_info);
        
        framebuffer_create_info.depth_stencil_attachment = layer->depth_stencil_attachment;
    }
    
    layer->framebuffer =
        mg_lowl_renderer_create_framebuffer(&framebuffer_create_info);
}

void mg_highl_renderer_begin_render_layer(mg_render_layer *layer, mg_vec4 clear_value)
{
    mg_render_pass_begin_info render_pass_begin_info = {
        .render_area = (mg_vec4) {0.0f, 0.0f, graphics_data.width, graphics_data.height},
        .clear_value = clear_value
    };
    
    mg_lowl_renderer_begin_render_pass(layer->render_pass,
		layer->framebuffer, &render_pass_begin_info);
    
    mg_lowl_renderer_viewport(0, 0, graphics_data.width, graphics_data.height);

    graphics_data.layers[graphics_data.layer_count] = layer;
    graphics_data.layer_count++;
}

void mg_highl_renderer_end_render_layer(mg_render_layer *layer)
{
    mg_lowl_renderer_end_render_pass();
    if (layer->internal_ub)
        mg_lowl_renderer_update_uniform_buffer(layer->internal_ub,
		sizeof(graphics_data.world_ubo_2d), &graphics_data.world_ubo_2d);
}

mg_render_layer *mg_highl_renderer_create_nonlit_layer_2d(void)
{
    mg_shader_source vertex_shader = {
        .code = graphics_data.shader_resources.nonlit_2d_vert,
        .size = graphics_data.shader_resources.nonlit_2d_vert_size
    };
    
    mg_shader_source fragment_shader = {
        .code = graphics_data.shader_resources.nonlit_2d_frag,
        .size = graphics_data.shader_resources.nonlit_2d_frag_size
    };
    
    mg_render_layer *layer = mg_highl_renderer_create_render_layer(vertex_shader, fragment_shader,
        false, sizeof(graphics_data.global_ubo), 0);
    
    return layer;
}

mg_render_layer *mg_highl_renderer_create_world_layer_2d(void)
{
    mg_shader_source vertex_shader = {
        .code = graphics_data.shader_resources.world_2d_vert,
        .size = graphics_data.shader_resources.world_2d_vert_size
    };
    
    mg_shader_source fragment_shader = {
        .code = graphics_data.shader_resources.world_2d_frag,
        .size = graphics_data.shader_resources.world_2d_frag_size
    };
    
    mg_render_layer *layer = mg_highl_renderer_create_render_layer(vertex_shader, fragment_shader,
		false, sizeof(graphics_data.global_ubo), sizeof(graphics_data.world_ubo_2d));
    
    return layer;
}

mg_render_layer *mg_highl_renderer_create_world_layer_3d(void)
{
    mg_shader_source vertex_shader = {
        .code = graphics_data.shader_resources.world_3d_vert,
        .size = graphics_data.shader_resources.world_3d_vert_size
    };
    
    mg_shader_source fragment_shader = {
        .code = graphics_data.shader_resources.world_3d_frag,
        .size = graphics_data.shader_resources.world_3d_frag_size
    };
    
    mg_render_layer *layer = mg_highl_renderer_create_render_layer(vertex_shader, fragment_shader,
		true, sizeof(graphics_data.global_ubo), 0);
    
    return layer;
}

void mg_highl_renderer_initialize(mg_highl_renderer_init_info *info)
{
    mg_swapchain_config_info swapchain_info;
    swapchain_info.format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB;
    swapchain_info.present_mode = MG_PRESENT_MODE_FIFO;
    swapchain_info.width = info->width;
    swapchain_info.height = info->height;

    mg_lowl_renderer_init_info renderer_info;
    renderer_info.platform = info->platform;
    renderer_info.type = info->type;
    renderer_info.swapchain_config_info = &swapchain_info;

	mg_lowl_renderer_initialize(&renderer_info);

    graphics_data.width = info->width;
    graphics_data.height = info->height;
    
    graphics_data.world_ubo_2d.resolution = (mg_vec2){ graphics_data.width, graphics_data.height };
    
    mg_shader_resources_t shader_resources;
    
    switch (info->type)
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
#if MG_PLATFORM_WINDOWS
	case MG_RENDERER_TYPE_DIRECT3D11:
		shader_resources.world_2d_vert = HLSL_WORLD_2D_VERT;
        shader_resources.world_2d_vert_size = sizeof(HLSL_WORLD_2D_VERT);
        shader_resources.world_2d_frag = HLSL_WORLD_2D_FRAG;
        shader_resources.world_2d_frag_size = sizeof(HLSL_WORLD_2D_FRAG);
        
        shader_resources.nonlit_2d_vert = HLSL_NONLIT_2D_VERT;
        shader_resources.nonlit_2d_vert_size = sizeof(HLSL_NONLIT_2D_VERT);
        shader_resources.nonlit_2d_frag = HLSL_NONLIT_2D_FRAG;
        shader_resources.nonlit_2d_frag_size = sizeof(HLSL_NONLIT_2D_FRAG);
        
        shader_resources.world_3d_vert = HLSL_WORLD_3D_VERT;
        shader_resources.world_3d_vert_size = sizeof(HLSL_WORLD_3D_VERT);
        shader_resources.world_3d_frag = HLSL_WORLD_3D_FRAG;
        shader_resources.world_3d_frag_size = sizeof(HLSL_WORLD_3D_FRAG);
        
        shader_resources.quad_vert = HLSL_QUAD_VERT;
        shader_resources.quad_vert_size = sizeof(HLSL_QUAD_VERT);
        shader_resources.quad_frag = HLSL_QUAD_FRAG;
        shader_resources.quad_frag_size = sizeof(HLSL_QUAD_FRAG);
        
        shader_resources.sprite_vert = HLSL_SPRITE_VERT;
        shader_resources.sprite_vert_size = sizeof(HLSL_SPRITE_VERT);
        shader_resources.sprite_frag = HLSL_SPRITE_FRAG;
        shader_resources.sprite_frag_size = sizeof(HLSL_SPRITE_FRAG);
        
        shader_resources.text_vert = HLSL_TEXT_VERT;
        shader_resources.text_vert_size = sizeof(HLSL_TEXT_VERT);
        shader_resources.text_frag = HLSL_TEXT_FRAG;
        shader_resources.text_frag_size = sizeof(HLSL_TEXT_FRAG);
        
        shader_resources.mesh_vert = HLSL_MESH_VERT;
        shader_resources.mesh_vert_size = sizeof(HLSL_MESH_VERT);
        shader_resources.mesh_frag = HLSL_MESH_FRAG;
        shader_resources.mesh_frag_size = sizeof(HLSL_MESH_FRAG);
		break;
#endif
    }
    
    graphics_data.shader_resources = shader_resources;
    
    const uint16_t indices[6] = {
        0, 1, 3, 1, 2, 3
    };
    
    graphics_data.index_buffer =
        mg_lowl_renderer_create_index_buffer(sizeof(indices), indices);
    
#pragma region SCREEN_DATA
    {
        const float vertices[16] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 1.0f
        };
        
        graphics_data.screen_vertex_buffer =
            mg_lowl_renderer_create_vertex_buffer(sizeof(vertices), vertices);
        
        mg_sampler_create_info sampler_create_info = {
            .mag_filter = MG_SAMPLER_FILTER_NEAREST,
            .min_filter = MG_SAMPLER_FILTER_NEAREST,
            .address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
        };
        
        graphics_data.screen_sampler =
            mg_lowl_renderer_create_sampler(&sampler_create_info);
    }
#pragma endregion
    
#pragma region QUAD_DATA
    {
        mg_shader_source vertex_shader = {
            .code = shader_resources.quad_vert,
            .size = shader_resources.quad_vert_size
        };
        
        mg_shader_source fragment_shader = {
            .code = shader_resources.quad_frag,
            .size = shader_resources.quad_frag_size
        };
        
        mg_vertex_attribute_info vertex_attributes[1] = {
            {
                .location = 0,
                .offset = 0,
                .format = MG_VERTEX_FORMAT_FLOAT2
            }
        };
        
        mg_vertex_layout_info vertex_layout = {
            .stride = 2 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 1
        };
        
        mg_pipeline_create_info pipeline_create_info = {
            .shader = {vertex_shader, fragment_shader},
            
            vertex_layout = vertex_layout,

            .primitive_topology = MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
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
            
            .push_constants_size = sizeof(graphics_data.quad_data.push_constant)
        };
        
        graphics_data.quad_data.pipeline =
            mg_lowl_renderer_create_pipeline(&pipeline_create_info);
        
        const float vertices[8] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };
        
        graphics_data.quad_data.vertex_buffer =
            mg_lowl_renderer_create_vertex_buffer(sizeof(vertices), vertices);
    }
#pragma endregion

#pragma region SPRITE_DATA
    {
        mg_shader_source vertex_shader = {
            .code = shader_resources.sprite_vert,
            .size = shader_resources.sprite_vert_size
        };
        
        mg_shader_source fragment_shader = {
            .code = shader_resources.sprite_frag,
            .size = shader_resources.sprite_frag_size
        };
        
        mg_vertex_attribute_info vertex_attributes[2] = {
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
        
        mg_vertex_layout_info vertex_layout = {
            .stride = 4 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 2
        };
        
        mg_pipeline_create_info pipeline_create_info = {
            .shader = {vertex_shader, fragment_shader},
            
            vertex_layout = vertex_layout,

            .primitive_topology = MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
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
            
            .push_constants_size = sizeof(graphics_data.sprite_data.push_constant)
        };
        
        graphics_data.sprite_data.pipeline =
            mg_lowl_renderer_create_pipeline(&pipeline_create_info);
        
        const float vertices[16] = {
            0.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f
        };
        
        graphics_data.sprite_data.vertex_buffer =
            mg_lowl_renderer_create_vertex_buffer(sizeof(vertices), vertices);
    }
#pragma endregion

#pragma region TEXT_DATA
    {
        mg_shader_source vertex_shader = {
            .code = shader_resources.text_vert,
            .size = shader_resources.text_vert_size
        };
        
        mg_shader_source fragment_shader = {
            .code = shader_resources.text_frag,
            .size = shader_resources.text_frag_size
        };
        
        mg_vertex_attribute_info vertex_attributes[2] = {
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
        
        mg_vertex_layout_info vertex_layout = {
            .stride = 4 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 2
        };
        
        mg_pipeline_create_info pipeline_create_info = {
            .shader = {vertex_shader, fragment_shader},
            
            vertex_layout = vertex_layout,

            .primitive_topology = MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
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
            
            .push_constants_size = sizeof(graphics_data.text_data.push_constant)
        };
        
        graphics_data.text_data.pipeline =
            mg_lowl_renderer_create_pipeline(&pipeline_create_info);
        
        for (uint32_t i = 0; i < MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS; i++)
        {
            graphics_data.text_data.vertex_buffers[i] =
                mg_lowl_renderer_create_dynamic_vertex_buffer(sizeof(graphics_data.text_data.vertices));
            
            graphics_data.text_data.index_buffers[i] =
                mg_lowl_renderer_create_dynamic_index_buffer(sizeof(graphics_data.text_data.indices));
        }
    }
#pragma endregion

#pragma region MESH_DATA_2D
    {
        mg_shader_source vertex_shader = {
            .code = shader_resources.quad_vert,
            .size = shader_resources.quad_vert_size
        };
        
        mg_shader_source fragment_shader = {
            .code = shader_resources.quad_frag,
            .size = shader_resources.quad_frag_size
        };
        
        mg_vertex_attribute_info vertex_attributes[2] = {
            {
                .location = 0,
                .offset = 0,
                .format = MG_VERTEX_FORMAT_FLOAT2
            },
            {
                .location = 2,
                .offset = 2 * sizeof(float),
                .format = MG_VERTEX_FORMAT_FLOAT2
            }
        };
        
        mg_vertex_layout_info vertex_layout = {
            .stride = 4 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 2
        };
        
        mg_pipeline_create_info pipeline_create_info = {
            .shader = {vertex_shader, fragment_shader},
            
            vertex_layout = vertex_layout,

            .primitive_topology = MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
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
            
            .push_constants_size = sizeof(graphics_data.mesh_data_2d.push_constant)
        };
        
        graphics_data.mesh_data_2d.pipeline =
            mg_lowl_renderer_create_pipeline(&pipeline_create_info);
    }
#pragma endregion

#pragma region MESH_DATA_3D
    {
        mg_shader_source vertex_shader = {
            .code = shader_resources.mesh_vert,
            .size = shader_resources.mesh_vert_size
        };
        
        mg_shader_source fragment_shader = {
            .code = shader_resources.mesh_frag,
            .size = shader_resources.mesh_frag_size
        };
        
        mg_vertex_attribute_info vertex_attributes[3] = {
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
        
        mg_vertex_layout_info vertex_layout = {
            .stride = 8 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 3
        };
        
        mg_pipeline_create_info pipeline_create_info = {
            .shader = {vertex_shader, fragment_shader},
            
            vertex_layout = vertex_layout,
            
            .primitive_topology = MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
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
            
            .push_constants_size = sizeof(graphics_data.mesh_data_3d.push_constant)
        };
        
        graphics_data.mesh_data_3d.pipeline =
            mg_lowl_renderer_create_pipeline(&pipeline_create_info);
    }
#pragma endregion
}

void mg_highl_renderer_shutdown(void)
{
    mg_lowl_renderer_wait();
    
    // Quad Data
    mg_lowl_renderer_destroy_vertex_buffer(graphics_data.quad_data.vertex_buffer);
    mg_lowl_renderer_destroy_pipeline(graphics_data.quad_data.pipeline);
    
    // Sprite Data
    mg_lowl_renderer_destroy_vertex_buffer(graphics_data.sprite_data.vertex_buffer);
    mg_lowl_renderer_destroy_pipeline(graphics_data.sprite_data.pipeline);
    
    // Text Data
    for (uint32_t i = 0; i < MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS; i++)
    {
        mg_lowl_renderer_destroy_dynamic_vertex_buffer(graphics_data.text_data.vertex_buffers[i]);
        mg_lowl_renderer_destroy_dynamic_index_buffer(graphics_data.text_data.index_buffers[i]);
    }

    mg_lowl_renderer_destroy_pipeline(graphics_data.text_data.pipeline);
    
    // Mesh Data
    mg_lowl_renderer_destroy_pipeline(graphics_data.mesh_data_2d.pipeline);
    //mg_lowl_renderer_destroy_pipeline(graphics_data.mesh_data_3d.pipeline);
    
    mg_lowl_renderer_destroy_sampler(graphics_data.screen_sampler);
    mg_lowl_renderer_destroy_vertex_buffer(graphics_data.screen_vertex_buffer);
    mg_lowl_renderer_destroy_index_buffer(graphics_data.index_buffer);

    mg_lowl_renderer_shutdown();
}

void mg_highl_renderer_resize(int32_t width, int32_t height)
{
    graphics_data.width = width;
    graphics_data.height = height;
    
    graphics_data.world_ubo_2d.resolution = (mg_vec2){ width, height };

    mg_swapchain_config_info config_info;
	config_info.format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB;
	config_info.present_mode = MG_PRESENT_MODE_FIFO;
	config_info.width = width;
	config_info.height = height;

    mg_lowl_renderer_configure_swapchain(&config_info);        
}

void mg_highl_renderer_begin(void)
{
    graphics_data.frame_data.bind_id = 0;
    graphics_data.frame_data.text_index = 0;
    graphics_data.layer_count = 0;
    graphics_data.world_ubo_2d.light_count = 0;

    mg_lowl_renderer_begin_frame();
}

void mg_highl_renderer_end(void)
{
    mg_render_pass_begin_info render_pass_begin_info = {
        .render_area = (mg_vec4) {0.0f, 0.0f, graphics_data.width, graphics_data.height},
        .clear_value = (mg_vec4) {0.0f, 0.0f, 0.0f, 1.0f}
    };
    
    mg_lowl_renderer_begin_default_render_pass(&render_pass_begin_info);
    mg_lowl_renderer_viewport(0, 0, graphics_data.width, graphics_data.height);

    for (uint32_t i = 0; i < graphics_data.layer_count; i++)
    {
        mg_render_layer *layer = graphics_data.layers[i];
        mg_lowl_renderer_bind_pipeline(layer->pipeline);
        mg_lowl_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT16);
        mg_lowl_renderer_bind_vertex_buffer(graphics_data.screen_vertex_buffer);
        if (layer->internal_ub)
            mg_lowl_renderer_bind_uniform_buffer(layer->internal_ub);
        
        mg_lowl_renderer_bind_image_array(layer->image_array);
        //mg_lowl_renderer_bind_image(layer->color_attachment, graphics_data.screen_sampler, 0);
        mg_lowl_renderer_draw_indexed(6, 0);
    }
    
    mg_lowl_renderer_end_render_pass();

    mg_lowl_renderer_end_frame();
    mg_lowl_renderer_present_frame();
}

void mg_highl_renderer_begin_background(mg_render_layer *layer, mg_vec4 clear_color)
{
    mg_highl_renderer_begin_render_layer(layer, clear_color);
    
    const int32_t width = graphics_data.width;
    const int32_t height = graphics_data.height;
    
    graphics_data.global_ubo.view_projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);
    
    mg_lowl_renderer_update_uniform_buffer(layer->global_ub,
        sizeof(graphics_data.global_ubo), &graphics_data.global_ubo);
    
    graphics_data.current_layer = layer;
}

void mg_highl_renderer_begin_world_2d(mg_render_layer *layer, mg_world_info_2d *world_info)
{
    mg_highl_renderer_begin_render_layer(layer, (mg_vec4) {0.0f, 0.0f, 0.0f, 0.0f});
    
    const int32_t width = graphics_data.width;
    const int32_t height = graphics_data.height;
    
    mg_mat4 projection;
    
    projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);
    projection = mg_mat4_translate(projection, (mg_vec3) { -world_info->camera_position.x / width * 2.0f, -world_info->camera_position.y / height * 2.0f, 0.0f });
    projection = mg_mat4_scale(projection, (mg_vec3) { world_info->camera_zoom.x, world_info->camera_zoom.y, 1.0f });

    graphics_data.world_ubo_2d.flag_enable_vignette = world_info->flags & MG_WORLD_FLAG_2D_ENABLE_VIGNETTE;
    graphics_data.world_ubo_2d.flag_enable_shadows = world_info->flags & MG_WORLD_FLAG_2D_ENABLE_SHADOWS;
    graphics_data.world_ubo_2d.flag_enable_lights = world_info->flags & MG_WORLD_FLAG_2D_ENABLE_LIGHTS;
    graphics_data.world_ubo_2d.shadow_data = (mg_vec4) {world_info->shadow_offset.x, world_info->shadow_offset.y, world_info->shadow_opacity, 0.0f};
    
    graphics_data.world_ubo_2d.global_light_data =
    (mg_vec4) { world_info->global_light_color.x, world_info->global_light_color.y, world_info->global_light_color.z,
        world_info->global_light_intensity};
    
    graphics_data.global_ubo.view_projection = projection;
    
    mg_lowl_renderer_update_uniform_buffer(layer->global_ub,
        sizeof(graphics_data.global_ubo), &graphics_data.global_ubo);
    
    graphics_data.current_layer = layer;
}

void mg_highl_renderer_begin_nonlit_2d(mg_render_layer *layer)
{
    mg_highl_renderer_begin_render_layer(layer, (mg_vec4) {0.0f, 0.0f, 0.0f, 0.0f});
    
    const int32_t width = graphics_data.width;
    const int32_t height = graphics_data.height;
    
    graphics_data.global_ubo.view_projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);
    
    mg_lowl_renderer_update_uniform_buffer(layer->global_ub,
        sizeof(graphics_data.global_ubo), &graphics_data.global_ubo);
    
    graphics_data.current_layer = layer;
}

void mg_highl_renderer_draw_rect_2d_internal(mg_mat4 model, mg_vec4 color)
{
    if (graphics_data.frame_data.bind_id != 1)
    {
        mg_lowl_renderer_bind_pipeline(graphics_data.quad_data.pipeline);
        mg_lowl_renderer_bind_uniform_buffer(graphics_data.current_layer->global_ub);
        mg_lowl_renderer_bind_vertex_buffer(graphics_data.quad_data.vertex_buffer);
        mg_lowl_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT16);
        graphics_data.frame_data.bind_id = 1;
    }
    
    graphics_data.quad_data.push_constant.model = model;
    graphics_data.quad_data.push_constant.color = color;
    
    mg_lowl_renderer_push_constants(sizeof(graphics_data.quad_data.push_constant), &graphics_data.quad_data.push_constant);
    
    mg_lowl_renderer_draw_indexed(6, 0);
}

void mg_highl_renderer_draw_sprite_2d_internal(mg_mat4 model, mg_vec4 color, mg_sprite *sprite)
{
    if (graphics_data.frame_data.bind_id != 2)
    {
        mg_lowl_renderer_bind_pipeline(graphics_data.sprite_data.pipeline);
        mg_lowl_renderer_bind_uniform_buffer(graphics_data.current_layer->global_ub);
        mg_lowl_renderer_bind_vertex_buffer(graphics_data.sprite_data.vertex_buffer);
        mg_lowl_renderer_bind_index_buffer(graphics_data.index_buffer, MG_INDEX_TYPE_UINT16);
        graphics_data.frame_data.bind_id = 2;
    }

    mg_lowl_renderer_bind_image_array(sprite->texture->image_array);
    //mg_lowl_renderer_bind_image(sprite->texture->image, sprite->texture->sampler, 0);
    
    graphics_data.sprite_data.push_constant.model = model;
    graphics_data.sprite_data.push_constant.color = color;
    graphics_data.sprite_data.push_constant.rect =
    (mg_vec4) { sprite->x, sprite->y, sprite->width, sprite->height };
    
    mg_lowl_renderer_push_constants(sizeof(graphics_data.sprite_data.push_constant), &graphics_data.sprite_data.push_constant);
    
    mg_lowl_renderer_draw_indexed(6, 0);
}

void mg_highl_renderer_draw_line_2d(mg_vec2 point1, mg_vec2 point2, float width, mg_vec4 color)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { 0.5f, 0.0f, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { mg_vec2_distance(point1, point2), width, 1.0f });
    model = mg_mat4_rotate_z(model, atan2f(point2.y - point1.y, point2.x - point1.x));
    model = mg_mat4_translate(model, (mg_vec3) { point1.x, point1.y, 0.0f });
    mg_highl_renderer_draw_rect_2d_internal(model, color);
}

void mg_highl_renderer_draw_text_2d_internal(mg_mat4 model, mg_vec4 color, const char *text, mg_font *font)
{
    if (graphics_data.frame_data.text_index >= MG_CONFIG_GRAPHICS_MAX_TEXT_DRAWS)
        return;

    if (graphics_data.frame_data.bind_id != 3)
    {
        mg_lowl_renderer_bind_pipeline(graphics_data.text_data.pipeline);
        mg_lowl_renderer_bind_uniform_buffer(graphics_data.current_layer->global_ub);
        graphics_data.frame_data.bind_id = 3;
    }

    mg_lowl_renderer_bind_image_array(font->texture->image_array);
    //mg_lowl_renderer_bind_image(font->texture->image, font->texture->sampler, 0);

    graphics_data.text_data.push_constant.model = model;
    graphics_data.text_data.push_constant.color = color;

    mg_lowl_renderer_push_constants(sizeof(graphics_data.text_data.push_constant), &graphics_data.text_data.push_constant);

    float x = 0;
    float y = 0;

    static const float font_height = MG_CONFIG_HIGHL_RENDERER_FONT_HEIGHT * 4.0f;

    uint32_t i;
    for (i = 0; i < strlen(text); i++)
    {
        switch (text[i])
        {
            case '\n':
                x = 0.0f;
                y += 1.0f;
                memset(&graphics_data.text_data.vertices[i * 16], 0, 16 * sizeof(float));
                break;
            case ' ':
                x += 0.5f;
                memset(&graphics_data.text_data.vertices[i * 16], 0, 16 * sizeof(float));
                break;
            default:
            {
                float _x = 0.0f, _y = 0.0f;
                stbtt_bakedchar b = font->cdata[(int32_t)text[i]];
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(font->cdata, 1024, 1024, (int32_t)text[i] - 32, &_x, &_y, &q, 0);

                float descent = q.y1 / font_height;

                graphics_data.text_data.vertices[i * 16] = x;
                graphics_data.text_data.vertices[i * 16 + 1] = y - (q.y1 - q.y0) / font_height + descent;
                graphics_data.text_data.vertices[i * 16 + 2] = q.s0 * 0.5f;
                graphics_data.text_data.vertices[i * 16 + 3] = q.t0;

                graphics_data.text_data.vertices[i * 16 + 4] = x + (q.x1 - q.x0) / font_height;
                graphics_data.text_data.vertices[i * 16 + 5] = y - (q.y1 - q.y0) / font_height + descent;
                graphics_data.text_data.vertices[i * 16 + 6] = q.s1 * 0.5f - 0.001f;
                graphics_data.text_data.vertices[i * 16 + 7] = q.t0;

                graphics_data.text_data.vertices[i * 16 + 8] = x + (q.x1 - q.x0) / font_height;
                graphics_data.text_data.vertices[i * 16 + 9] = y + descent;
                graphics_data.text_data.vertices[i * 16 + 10] = q.s1 * 0.5f - 0.001f;
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

                x += (q.x1 - q.x0 + 9.0f) / font_height;
            }
        }
    }

    mg_lowl_renderer_update_dynamic_vertex_buffer(graphics_data.text_data.vertex_buffers[graphics_data.frame_data.text_index], i * 16 * sizeof(float), graphics_data.text_data.vertices);
    mg_lowl_renderer_update_dynamic_index_buffer(graphics_data.text_data.index_buffers[graphics_data.frame_data.text_index], i * 6 * sizeof(uint32_t), graphics_data.text_data.indices);

    mg_lowl_renderer_bind_dynamic_vertex_buffer(graphics_data.text_data.vertex_buffers[graphics_data.frame_data.text_index]);
    mg_lowl_renderer_bind_dynamic_index_buffer(graphics_data.text_data.index_buffers[graphics_data.frame_data.text_index], MG_INDEX_TYPE_UINT16);

    mg_lowl_renderer_draw_indexed(i * 6, 0);

    graphics_data.frame_data.text_index++;
}

void mg_highl_renderer_draw_mesh_2d_internal(mg_mat4 model, mg_vec4 color, mg_mesh *mesh, mg_texture *texture)
{
    if (graphics_data.frame_data.bind_id != 4)
    {
        mg_lowl_renderer_bind_pipeline(graphics_data.mesh_data_2d.pipeline);
        mg_lowl_renderer_bind_uniform_buffer(graphics_data.current_layer->global_ub);
        graphics_data.frame_data.bind_id = 4;
    }
    
    mg_lowl_renderer_bind_vertex_buffer(mesh->vertex_buffer);
    mg_lowl_renderer_bind_index_buffer(mesh->index_buffer, MG_INDEX_TYPE_UINT32);
    
    mg_lowl_renderer_bind_image_array(texture->image_array);
    //mg_lowl_renderer_bind_image(texture->image, texture->sampler, 0);
    
    graphics_data.mesh_data_2d.push_constant.model = model;
    graphics_data.mesh_data_2d.push_constant.color = color;
    
    mg_lowl_renderer_push_constants(sizeof(graphics_data.mesh_data_2d.push_constant), &graphics_data.mesh_data_2d.push_constant);
    
    mg_lowl_renderer_draw_indexed(mesh->index_count, 0);
}

void mg_highl_renderer_draw_rect_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_highl_renderer_draw_rect_2d_internal(model, color);
}

void mg_highl_renderer_draw_sprite_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, mg_sprite *sprite)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_highl_renderer_draw_sprite_2d_internal(model, color, sprite);
}

void mg_highl_renderer_draw_text_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, const char *text, mg_font *font)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_highl_renderer_draw_text_2d_internal(model, color, text, font);
}

void mg_highl_renderer_draw_mesh_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, mg_mesh *mesh, mg_texture *texture)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_highl_renderer_draw_mesh_2d_internal(model, color, mesh, texture);
}

void mg_highl_renderer_draw_rotated_rect_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_highl_renderer_draw_rect_2d_internal(model, color);
}

void mg_highl_renderer_draw_rotated_sprite_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, mg_sprite *sprite)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_highl_renderer_draw_sprite_2d_internal(model, color, sprite);
}

void mg_highl_renderer_draw_rotated_text_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, const char *text, mg_font *font)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_highl_renderer_draw_text_2d_internal(model, color, text, font);
}

void mg_highl_renderer_draw_rotated_mesh_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, mg_mesh *mesh, mg_texture *texture)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_highl_renderer_draw_mesh_2d_internal(model, color, mesh, texture);
}

// TODO (box): Fix this
/*void mg_highl_renderer_draw_reflective_rect(mg_vec2 position, mg_vec2 scale, mg_vec4 color)
{
    if (graphics_data.frame_data.bind_id != 3)
    {
        mg_lowl_renderer_bind_pipeline(graphics_data.sprite_data.pipeline);
        mg_lowl_renderer_bind_vertex_buffer(graphics_data.sprite_data.vertex_buffer);
        graphics_data.frame_data.bind_id = 3;
    }

    mg_lowl_renderer_bind_image(graphics_data.world_data.image, graphics_data.sprite_data.pipeline);

    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_scale(model, (mg_vec3){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3){ position.x, position.y, 0.0f });

    graphics_data.sprite_data.push_constant.model = model;
    graphics_data.sprite_data.push_constant.color = color;
    graphics_data.sprite_data.push_constant.rect =
        (mg_vec4) { 0, 0, 0.3f, 0.3f };

    mg_lowl_renderer_push_constants(sizeof(graphics_data.sprite_data.push_constant), &graphics_data.sprite_data.push_constant);

    mg_lowl_renderer_draw_indexed(6, 0);
}*/

void mg_highl_renderer_draw_point_light_2d(mg_vec2 position, float scale, float intensity, mg_vec3 color)
{
    if (graphics_data.world_ubo_2d.light_count >= MG_HIGHL_RENDERER_2D_MAX_LIGHT_COUNT)
        graphics_data.world_ubo_2d.light_count = 0;
    
    graphics_data.world_ubo_2d.light_transform[graphics_data.world_ubo_2d.light_count] =
    (mg_vec4) { position.x, position.y, scale, intensity };
    graphics_data.world_ubo_2d.light_colors[graphics_data.world_ubo_2d.light_count] =
    (mg_vec4) { color.r, color.g, color.b, 1.0f };
    
    graphics_data.world_ubo_2d.light_count++;
}

void mg_highl_renderer_begin_world_3d(mg_render_layer *layer)
{
    mg_highl_renderer_begin_render_layer(layer, (mg_vec4) {0.0f, 0.0f, 0.0f, 0.0f});
    
    graphics_data.global_ubo.view_projection = mg_mat4_perspective(45 * MG_DEG2RAD, (float)graphics_data.width / (float)graphics_data.height, 0.001f, 10.0f);
    graphics_data.global_ubo.view_projection = mg_mat4_multiply(mg_mat4_look_at(mg_vec3_new(0.0f, 0.0f, 1.0f), mg_vec3_new(0.0f, 0.0f, 3.0f), mg_vec3_new(0.0f, 1.0f, 0.0f)), graphics_data.global_ubo.view_projection);
    
    mg_lowl_renderer_update_uniform_buffer(layer->global_ub,
        sizeof(graphics_data.global_ubo), &graphics_data.global_ubo);
    
    graphics_data.current_layer = layer;
}

void mg_highl_renderer_end_world_3d(mg_render_layer *layer)
{
    mg_lowl_renderer_end_render_pass();
}

void mg_highl_renderer_draw_mesh_3d(mg_mat4 model, mg_mesh *mesh, mg_texture *texture)
{
    if (graphics_data.frame_data.bind_id != 5)
    {
        mg_lowl_renderer_bind_pipeline(graphics_data.mesh_data_3d.pipeline);
        mg_lowl_renderer_bind_uniform_buffer(graphics_data.current_layer->global_ub);
        graphics_data.frame_data.bind_id = 5;
    }
    
    mg_lowl_renderer_bind_vertex_buffer(mesh->vertex_buffer);
    mg_lowl_renderer_bind_index_buffer(mesh->index_buffer, MG_INDEX_TYPE_UINT32);

    mg_lowl_renderer_bind_image_array(texture->image_array);

    /*mg_lowl_renderer_bind_image(texture->image, texture->sampler, 0);
    mg_lowl_renderer_bind_image(texture2->image, texture2->sampler, 1);
    mg_lowl_renderer_bind_image(texture2->image, texture2->sampler, 2);
    mg_lowl_renderer_bind_image(texture2->image, texture2->sampler, 3);
    mg_lowl_renderer_bind_image(texture2->image, texture2->sampler, 4);
    mg_lowl_renderer_bind_image(texture2->image, texture2->sampler, 5);
    mg_lowl_renderer_bind_image(texture2->image, texture2->sampler, 6);
    mg_lowl_renderer_bind_image(texture2->image, texture2->sampler, 7);*/

    graphics_data.mesh_data_3d.push_constant.model = model;
    graphics_data.mesh_data_3d.push_constant.color = mg_vec4_new(1.0f, 1.0f, 1.0f, 1.0f);
    mg_lowl_renderer_push_constants(sizeof(graphics_data.mesh_data_3d.push_constant), &graphics_data.mesh_data_3d.push_constant);
    
    mg_lowl_renderer_draw_indexed(mesh->index_count, 0);
}