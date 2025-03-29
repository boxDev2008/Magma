#include "highl_renderer.h"

#include <magma/math/mat4.h>
#include <magma/math/math.h>

#include "shaders/post_process/raw.h"
#include "shaders/post_process/world_2d.h"

#include "shaders/sprite.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MG_HIGHL_RENDERER_2D_MAX_GEOMETRY_COUNT (1 << 14)
#define MG_HIGHL_RENDERER_2D_MAX_LIGHT_COUNT 16

typedef struct mg_world_ubo_2d
{
    mg_vec4 light_transform[MG_HIGHL_RENDERER_2D_MAX_LIGHT_COUNT];
    mg_vec4 light_colors[MG_HIGHL_RENDERER_2D_MAX_LIGHT_COUNT];
    mg_vec4 global_light_data;
    //mg_vec4 shadow_data;
    mg_vec4 resolution;
    int32_t light_count;
    int32_t flag_enable_vignette;
    int32_t flag_enable_lights;
    int32_t flag_enable_shadows;
}
mg_world_ubo_2d;

typedef struct mg_batch_vertex
{
    mg_vec2 position;
    mg_vec2 texcoord;
    mg_vec4 color;
    float tex_id;
}
mg_batch_vertex;

typedef struct mg_batch_quad
{
    mg_batch_vertex vertices[4];
}
mg_batch_quad;

typedef struct mg_hlgfx_data
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
    
    mg_world_ubo_2d world_ubo_2d;
    
    struct
    {
        mg_pipeline pipeline;
        mg_dynamic_vertex_buffer vb;
        mg_index_buffer ib;
        
        mg_batch_quad quads[MG_HIGHL_RENDERER_2D_MAX_GEOMETRY_COUNT];
        uint32_t quad_count;
        uint32_t current_offset;

        mg_image images[MG_CONFIG_MAX_BINDABLE_IMAGES];
        mg_sampler samplers[MG_CONFIG_MAX_BINDABLE_IMAGES];
        mg_image_array image_array;
        uint8_t image_count;
    }
    sprite_batch;
    
    mg_render_layer *current_layer;

    mg_renderer_type renderer_type;
    int32_t width, height;
}
mg_hlgfx_data;

static mg_hlgfx_data *rdata;

mg_render_layer *mg_hlgfx_create_render_layer(mg_shader shader,
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
        .width = rdata->width,
        .height = rdata->height
    };
    
    layer->color_attachment =
        mgfx_create_image(&color_attachment_info);

    layer->image_array = mgfx_create_image_array();
    mgfx_update_image_array(layer->image_array, &layer->color_attachment, &rdata->screen_sampler, 1);

    mg_framebuffer_create_info framebuffer_info = {
        .width = rdata->width,
        .height = rdata->height,
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
            .width = rdata->width,
            .height = rdata->height
        };
        
        framebuffer_info.depth_stencil_attachment = layer->depth_stencil_attachment =
            mgfx_create_image(&depth_stencil_attachment_create_info);
    }
    
    layer->render_pass =
        mgfx_create_render_pass(&render_pass_info);
    
    framebuffer_info.render_pass = layer->render_pass;
    
    layer->framebuffer =
        mgfx_create_framebuffer(&framebuffer_info);
    
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
        .shader = shader,
        
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
        mgfx_create_pipeline(&pipeline_create_info);

    return layer;
}

void mg_hlgfx_destroy_render_layer(mg_render_layer *layer)
{
    mgfx_destroy_framebuffer(layer->framebuffer);
    mgfx_destroy_image(layer->color_attachment);

    mgfx_destroy_image_array(layer->image_array);
    
    if (layer->depth_stencil_attachment)
        mgfx_destroy_image(layer->depth_stencil_attachment);
    
    mgfx_destroy_pipeline(layer->pipeline);
    mgfx_destroy_render_pass(layer->render_pass);
    
    free(layer);
}

void mg_hlgfx_resize_render_layer(mg_render_layer *layer, int32_t width, int32_t height)
{
    mgfx_destroy_framebuffer(layer->framebuffer);
    mgfx_destroy_image(layer->color_attachment);

    mg_image_create_info color_attachment_info = {
        .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
        .type = MG_IMAGE_TYPE_2D,
        .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
        .width = width,
        .height = height
    };
    
    layer->color_attachment =
        mgfx_create_image(&color_attachment_info);

    mgfx_update_image_array(layer->image_array, &layer->color_attachment, &rdata->screen_sampler, 1);

    mg_framebuffer_create_info framebuffer_create_info = {
        .width = width,
        .height = height,
        .render_pass = layer->render_pass,
        .color_attachment = layer->color_attachment
    };
    
    if (layer->depth_stencil_attachment)
    {
        mgfx_destroy_image(layer->depth_stencil_attachment);

        mg_image_create_info depth_stencil_attachment_create_info = {
            .format = MG_PIXEL_FORMAT_D32_SFLOAT,
            .type = MG_IMAGE_TYPE_2D,
            .usage = MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT,
            .width = width,
            .height = height
        };
        
        layer->depth_stencil_attachment =
            mgfx_create_image(&depth_stencil_attachment_create_info);
        
        framebuffer_create_info.depth_stencil_attachment = layer->depth_stencil_attachment;
    }
    
    layer->framebuffer =
        mgfx_create_framebuffer(&framebuffer_create_info);
}

void mg_hlgfx_begin_render_layer(mg_render_layer *layer, mg_vec4 clear_value)
{
    mg_render_pass_begin_info render_pass_begin_info = {
        .render_area = (mg_vec4) {0.0f, 0.0f, rdata->width, rdata->height},
        .clear_value = clear_value
    };
    
    mgfx_begin_render_pass(layer->render_pass,
		layer->framebuffer, &render_pass_begin_info);
    
    mgfx_viewport(0, 0, rdata->width, rdata->height);

    rdata->layers[rdata->layer_count] = layer;
    rdata->layer_count++;
}

void mg_hlgfx_end_render_layer(void)
{
    mgfx_end_render_pass();
}

mg_render_layer *mg_hlgfx_create_raw_layer_2d(void)
{
    mg_render_layer *layer = mg_hlgfx_create_render_layer(get_raw_shader(rdata->renderer_type),
        false, sizeof(rdata->global_ubo), 0);
    
    return layer;
}

mg_render_layer *mg_hlgfx_create_world_layer_2d(void)
{
    mg_render_layer *layer = mg_hlgfx_create_render_layer(get_world_2d_shader(rdata->renderer_type),
		false, sizeof(rdata->global_ubo), sizeof(rdata->world_ubo_2d));
    
    return layer;
}

mg_render_layer *mg_hlgfx_create_world_layer_3d(void)
{
    mg_render_layer *layer = mg_hlgfx_create_render_layer(get_raw_shader(rdata->renderer_type),
		true, sizeof(rdata->global_ubo), 0);
    
    return layer;
}

void mg_hlgfx_begin_world_2d(mg_render_layer *layer, mg_vec4 clear_color, mg_world_info_2d *world_info)
{
    mg_hlgfx_begin_render_layer(layer, clear_color);
    
    const int32_t width = rdata->width;
    const int32_t height = rdata->height;
    
    mg_mat4 projection;
    
    projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);
    projection = mg_mat4_translate(projection, (mg_vec3) { -world_info->camera_position.x / width * 2.0f, -world_info->camera_position.y / height * 2.0f, 0.0f });
    projection = mg_mat4_scale(projection, (mg_vec3) { world_info->camera_zoom.x, world_info->camera_zoom.y, 1.0f });

    rdata->world_ubo_2d.flag_enable_vignette = world_info->flags & MG_WORLD_FLAG_2D_ENABLE_VIGNETTE;
    //rdata->world_ubo_2d.flag_enable_shadows = world_info->flags & MG_WORLD_FLAG_2D_ENABLE_SHADOWS;
    rdata->world_ubo_2d.flag_enable_lights = world_info->flags & MG_WORLD_FLAG_2D_ENABLE_LIGHTS;
    //rdata->world_ubo_2d.shadow_data = (mg_vec4) {world_info->shadow_offset.x, world_info->shadow_offset.y, world_info->shadow_opacity, 0.0f};
    
    rdata->world_ubo_2d.global_light_data =
    (mg_vec4) { world_info->global_light_color.x, world_info->global_light_color.y, world_info->global_light_color.z,
        world_info->global_light_intensity};
    
    rdata->global_ubo.view_projection = projection;

    rdata->current_layer = layer;
}

void mg_hlgfx_begin_world_3d(mg_render_layer *layer, mg_vec4 clear_color)
{
    mg_hlgfx_begin_render_layer(layer, clear_color);
    
    rdata->global_ubo.view_projection = mg_mat4_perspective(45 * MG_DEG2RAD, (float)rdata->width / (float)rdata->height, 0.001f, 10.0f);
    rdata->global_ubo.view_projection = mg_mat4_multiply(mg_mat4_look_at(mg_vec3_new(0.0f, 0.0f, 1.0f), mg_vec3_new(0.0f, 0.0f, 3.0f), mg_vec3_new(0.0f, 1.0f, 0.0f)), rdata->global_ubo.view_projection);

    rdata->current_layer = layer;
}

void mg_hlgfx_begin_raw_2d(mg_render_layer *layer, mg_vec4 clear_color)
{
    mg_hlgfx_begin_render_layer(layer, clear_color);
    
    const int32_t width = rdata->width;
    const int32_t height = rdata->height;
    
    rdata->global_ubo.view_projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);

    rdata->current_layer = layer;
}

void mg_hlgfx_end_world_2d(mg_render_layer *layer)
{
    mg_hlgfx_end_render_layer();
}

void mg_hlgfx_end_world_3d(void)
{
    mg_hlgfx_end_render_layer();
}

void mg_hlgfx_end_raw_2d(void)
{
    mg_hlgfx_end_render_layer();
}

void mg_hlgfx_submit_sprite_batch(void)
{
    mgfx_update_dynamic_vertex_buffer(rdata->sprite_batch.vb,
        rdata->sprite_batch.quad_count * sizeof(mg_batch_quad), rdata->sprite_batch.quads);
    mgfx_bind_pipeline(rdata->sprite_batch.pipeline);
    mgfx_bind_dynamic_vertex_buffer(rdata->sprite_batch.vb);
    mgfx_bind_index_buffer(rdata->sprite_batch.ib, MG_INDEX_TYPE_UINT32);
	if (rdata->sprite_batch.image_array)
    	mgfx_bind_image_array(rdata->sprite_batch.image_array);
    mgfx_bind_uniforms(0, sizeof(rdata->global_ubo), &rdata->global_ubo);

    uint32_t offset = rdata->sprite_batch.quad_count * 6;

    mgfx_draw_indexed(offset - rdata->sprite_batch.current_offset, rdata->sprite_batch.current_offset, 0);
    rdata->sprite_batch.current_offset = offset;
}

void mg_hlgfx_initialize(mg_hlgfx_init_info *info)
{
    rdata = (mg_hlgfx_data*)calloc(1, sizeof(mg_hlgfx_data));

    mg_swapchain_config_info swapchain_info;
    swapchain_info.format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB;
    swapchain_info.present_mode = MG_PRESENT_MODE_FIFO;
    swapchain_info.width = info->width;
    swapchain_info.height = info->height;

    mgfx_init_info renderer_info;
    renderer_info.platform = info->platform;
    renderer_info.type = info->type;
    renderer_info.swapchain_config_info = &swapchain_info;

	mgfx_initialize(&renderer_info);

    rdata->width = info->width;
    rdata->height = info->height;
    rdata->renderer_type = info->type;
    
    rdata->world_ubo_2d.resolution.x = rdata->width;
    rdata->world_ubo_2d.resolution.y = rdata->height;
    
#pragma region SCREEN_DATA
    {
        const float vertices[16] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 1.0f
        };
        
        rdata->screen_vertex_buffer =
            mgfx_create_vertex_buffer(sizeof(vertices), vertices);

        const uint16_t indices[6] = {
            0, 1, 3, 1, 2, 3
        };
        
        rdata->index_buffer =
            mgfx_create_index_buffer(sizeof(indices), indices);

        mg_sampler_create_info sampler_create_info = {
            .mag_filter = MG_SAMPLER_FILTER_NEAREST,
            .min_filter = MG_SAMPLER_FILTER_NEAREST,
            .address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
        };
        
        rdata->screen_sampler =
            mgfx_create_sampler(&sampler_create_info);
    }
#pragma endregion

#pragma region BATCH_DATA
    {
        mg_vertex_attribute_info vertex_attributes[4] = {
            {
                .location = 0,
                .offset = 0,
                .format = MG_VERTEX_FORMAT_FLOAT2
            },
            {
                .location = 1,
                .offset = 2 * sizeof(float),
                .format = MG_VERTEX_FORMAT_FLOAT2
            },
            {
                .location = 2,
                .offset = 4 * sizeof(float),
                .format = MG_VERTEX_FORMAT_FLOAT4
            },
            {
                .location = 3,
                .offset = 8 * sizeof(float),
                .format = MG_VERTEX_FORMAT_FLOAT
            }
        };
        
        mg_vertex_layout_info vertex_layout = {
            .stride = 9 * sizeof(float),
            .attributes = vertex_attributes,
            .attribute_count = 4
        };
        
        mg_pipeline_create_info pipeline_create_info = {
            .shader = get_sprite_shader(rdata->renderer_type),
            
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
        };
        
        rdata->sprite_batch.pipeline =
            mgfx_create_pipeline(&pipeline_create_info);

        rdata->sprite_batch.vb =
            mgfx_create_dynamic_vertex_buffer(MG_HIGHL_RENDERER_2D_MAX_GEOMETRY_COUNT * sizeof(mg_batch_quad));
        
        uint32_t indices[MG_HIGHL_RENDERER_2D_MAX_GEOMETRY_COUNT * 6];
        
        for (uint32_t i = 0; i < MG_HIGHL_RENDERER_2D_MAX_GEOMETRY_COUNT; i++)
        {
            indices[i * 6 ] = i * 4;
            indices[i * 6 + 1] = i * 4 + 1;
            indices[i * 6 + 2] = i * 4 + 3;
            indices[i * 6 + 3] = i * 4 + 1;
            indices[i * 6 + 4] = i * 4 + 2;
            indices[i * 6 + 5] = i * 4 + 3;
        }
        rdata->sprite_batch.ib =
            mgfx_create_index_buffer(sizeof(indices), indices);
    }
#pragma endregion
}

void mg_hlgfx_shutdown(void)
{
    mgfx_wait();
    
    // Sprite Batch Data
    mgfx_destroy_dynamic_vertex_buffer(rdata->sprite_batch.vb);
    mgfx_destroy_index_buffer(rdata->sprite_batch.ib);
    mgfx_destroy_pipeline(rdata->sprite_batch.pipeline);

	if (rdata->sprite_batch.image_array)
    	mgfx_destroy_image_array(rdata->sprite_batch.image_array);

    mgfx_destroy_sampler(rdata->screen_sampler);
    mgfx_destroy_vertex_buffer(rdata->screen_vertex_buffer);
    mgfx_destroy_index_buffer(rdata->index_buffer);

    mgfx_shutdown();

    free(rdata);
}

void mg_hlgfx_resize(int32_t width, int32_t height)
{
    rdata->width = width;
    rdata->height = height;
    
    rdata->world_ubo_2d.resolution.x = rdata->width;
    rdata->world_ubo_2d.resolution.y = rdata->height;

    mg_swapchain_config_info config_info;
	config_info.format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB;
	config_info.present_mode = MG_PRESENT_MODE_FIFO;
	config_info.width = width;
	config_info.height = height;

    mgfx_configure_swapchain(&config_info);        
}

void mg_hlgfx_begin(void)
{
    rdata->layer_count = 0;
    rdata->world_ubo_2d.light_count = 0;

    rdata->sprite_batch.quad_count = 0;
    rdata->sprite_batch.current_offset = 0;

    mgfx_begin();
}

void mg_hlgfx_end(void)
{
    mg_render_pass_begin_info render_pass_begin_info = {
        .render_area = (mg_vec4) {0.0f, 0.0f, rdata->width, rdata->height},
        .clear_value = (mg_vec4) {0.0f, 0.0f, 0.0f, 1.0f}
    };
    
    mgfx_begin_default_render_pass(&render_pass_begin_info);
    mgfx_viewport(0, 0, rdata->width, rdata->height);

    for (uint32_t i = 0; i < rdata->layer_count; i++)
    {
        mg_render_layer *layer = rdata->layers[i];
        mgfx_bind_pipeline(layer->pipeline);
        mgfx_bind_index_buffer(rdata->index_buffer, MG_INDEX_TYPE_UINT16);
        mgfx_bind_vertex_buffer(rdata->screen_vertex_buffer);
        mgfx_bind_uniforms(0, sizeof(rdata->world_ubo_2d), &rdata->world_ubo_2d);
        
        mgfx_bind_image_array(layer->image_array);
        //mgfx_bind_image(layer->color_attachment, rdata->screen_sampler, 0);
        mgfx_draw_indexed(6, 0, 0);
    }
    
    mgfx_end_render_pass();

    mgfx_end();
}

void mg_hlgfx_draw_rect_2d_internal(mg_mat4 model, mg_vec4 color)
{
    const mg_vec4 v1 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v2 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v3 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 1.0f, 0.0f, 1.0f });
    const mg_vec4 v4 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 1.0f, 0.0f, 1.0f });

    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {v1.x, v1.y, 0.0f, 0.0f, color.r, color.g, color.b, color.a, -1},
        {v2.x, v2.y, 1.0f, 0.0f, color.r, color.g, color.b, color.a, -1},
        {v3.x, v3.y, 1.0f, 1.0f, color.r, color.g, color.b, color.a, -1},
        {v4.x, v4.y, 0.0f, 1.0f, color.r, color.g, color.b, color.a, -1}
    }};
}

void mg_hlgfx_draw_sprite_2d_internal(mg_mat4 model, mg_vec4 color, mg_sprite *sprite)
{
    const uint8_t tex_id = sprite->texture->id;

    const mg_vec4 v1 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v2 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v3 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 1.0f, 0.0f, 1.0f });
    const mg_vec4 v4 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 1.0f, 0.0f, 1.0f });

    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {v1.x, v1.y, sprite->x, sprite->y, color.r, color.g, color.b, color.a, tex_id},
        {v2.x, v2.y, sprite->x + sprite->width, sprite->y, color.r, color.g, color.b, color.a, tex_id},
        {v3.x, v3.y, sprite->x + sprite->width, sprite->y + sprite->height, color.r, color.g, color.b, color.a, tex_id},
        {v4.x, v4.y, sprite->x, sprite->y + sprite->height, color.r, color.g, color.b, color.a, tex_id}
    }};
}

void mg_hlgfx_draw_rect_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color)
{
    const mg_vec2 pivot_offset = (mg_vec2) { pivot.x * scale.x, pivot.y * scale.y };
    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {position.x - pivot_offset.x, position.y - pivot_offset.y, 0.0f, 0.0f, color.r, color.g, color.b, color.a, -1},
        {position.x + scale.x - pivot_offset.x, position.y - pivot_offset.y, 0.1f, 0.0f, color.r, color.g, color.b, color.a, -1},
        {position.x + scale.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, 1.0f, 1.0f, color.r, color.g, color.b, color.a, -1},
        {position.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, 0.0f, 1.0f, color.r, color.g, color.b, color.a, -1}
    }};
}

void mg_hlgfx_draw_sprite_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, mg_sprite *sprite)
{
    const mg_vec2 pivot_offset = (mg_vec2) { pivot.x * scale.x, pivot.y * scale.y };
    const uint8_t tex_id = sprite->texture->id;
    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {position.x - pivot_offset.x, position.y - pivot_offset.y, sprite->x, sprite->y, color.r, color.g, color.b, color.a, tex_id},
        {position.x + scale.x - pivot_offset.x, position.y - pivot_offset.y, sprite->x + sprite->width, sprite->y, color.r, color.g, color.b, color.a, tex_id},
        {position.x + scale.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, sprite->x + sprite->width, sprite->y + sprite->height, color.r, color.g, color.b, color.a, tex_id},
        {position.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, sprite->x, sprite->y + sprite->height, color.r, color.g, color.b, color.a, tex_id}
    }};
}

void mg_hlgfx_draw_rotated_rect_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_hlgfx_draw_rect_2d_internal(model, color);
}

void mg_hlgfx_draw_rotated_sprite_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, mg_sprite *sprite)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3) { -pivot.x, -pivot.y, 0.0f });
    model = mg_mat4_scale(model, (mg_vec3) { scale.x, scale.y, 1.0f });
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_translate(model, (mg_vec3) { position.x, position.y, 0.0f });
    mg_hlgfx_draw_sprite_2d_internal(model, color, sprite);
}

// TODO (box): Fix this
/*void mg_hlgfx_draw_reflective_rect(mg_vec2 position, mg_vec2 scale, mg_vec4 color)
{
    if (rdata->frame_data.bind_id != 3)
    {
        mgfx_bind_pipeline(rdata->sprite_data.pipeline);
        mgfx_bind_vertex_buffer(rdata->sprite_data.vertex_buffer);
        rdata->frame_data.bind_id = 3;
    }

    mgfx_bind_image(rdata->world_data.image, rdata->sprite_data.pipeline);

    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_scale(model, (mg_vec3){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3){ position.x, position.y, 0.0f });

    rdata->sprite_data.push_constant.model = model;
    rdata->sprite_data.push_constant.color = color;
    rdata->sprite_data.push_constant.rect =
        (mg_vec4) { 0, 0, 0.3f, 0.3f };

    mgfx_push_constants(sizeof(rdata->sprite_data.push_constant), &rdata->sprite_data.push_constant);

    mgfx_draw_indexed(6, 0);
}*/

void mg_hlgfx_draw_point_light_2d(mg_vec2 position, float scale, float intensity, mg_vec3 color)
{
    if (rdata->world_ubo_2d.light_count >= MG_HIGHL_RENDERER_2D_MAX_LIGHT_COUNT)
        rdata->world_ubo_2d.light_count = 0;
    
    if (rdata->renderer_type == MG_RENDERER_TYPE_DIRECT3D11)
        position.y = position.y * -1.0f + 1.0f;
    
    rdata->world_ubo_2d.light_transform[rdata->world_ubo_2d.light_count] =
    (mg_vec4) { position.x, position.y, scale, intensity };
    rdata->world_ubo_2d.light_colors[rdata->world_ubo_2d.light_count] =
    (mg_vec4) { color.r, color.g, color.b, 1.0f };
    
    rdata->world_ubo_2d.light_count++;
}

/*void mg_hlgfx_draw_mesh(mg_mat4 model, mg_mesh *mesh, mg_texture *texture)
{
    if (rdata->frame_data.bind_id != 5)
    {
        mgfx_bind_pipeline(rdata->mesh_data.pipeline);
        rdata->frame_data.bind_id = 5;
    }
    
    if (rdata->frame_data.prev_layer != rdata->current_layer)
    {
        mgfx_bind_uniform_buffer(rdata->current_layer->global_ub);
        rdata->frame_data.prev_layer = rdata->current_layer;
    }

    mgfx_bind_vertex_buffer(mesh->vertex_buffer);
    mgfx_bind_index_buffer(mesh->index_buffer, MG_INDEX_TYPE_UINT32);

    //mgfx_bind_image_array(texture->image_array);

    rdata->mesh_data.push_constant.model = model;
    rdata->mesh_data.push_constant.color = mg_vec4_new(1.0f, 1.0f, 1.0f, 1.0f);
    mgfx_push_constants(sizeof(rdata->mesh_data.push_constant), &rdata->mesh_data.push_constant);
    
    mgfx_draw_indexed(mesh->index_count, 0);
}*/

mg_texture mg_hlgfx_add_texture(uint32_t width, uint32_t height, mg_sampler_filter filter, mg_pixel_format format, void *data)
{
    assert (rdata->sprite_batch.image_count < MG_CONFIG_MAX_BINDABLE_IMAGES);

    mg_texture texture;
    texture.width = width;
    texture.height = height;
    texture.id = rdata->sprite_batch.image_count++;

    mg_image_create_info create_info = {
        .format = format,
        .type = MG_IMAGE_TYPE_2D,
        .width = width,
        .height = height,
    };

    rdata->sprite_batch.images[texture.id] = mgfx_create_image(&create_info);

    mg_sampler_create_info sampler_create_info = {
        .mag_filter = filter,
        .min_filter = filter,
        .address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
    };

    rdata->sprite_batch.samplers[texture.id] = mgfx_create_sampler(&sampler_create_info);

    mg_image_write_info texture_image_write_info = {
        .format = format,
        .width = width,
        .height = height,
        .data = data
    };

    mgfx_update_image(rdata->sprite_batch.images[texture.id], &texture_image_write_info);

    return texture;
}

mg_texture mg_hlgfx_add_texture_from_file(const char *file_name, mg_sampler_filter filter, mg_pixel_format format)
{
    int32_t width, height, channels;
    uint8_t *data = stbi_load(file_name, &width, &height, &channels, STBI_rgb_alpha);
	if (!data)
	{
		printf("Failed to load texture from file %s\n", file_name);
		return;
	}
    mg_texture texture = mg_hlgfx_add_texture(width, height, filter, format, data);
    stbi_image_free(data);
    return texture;
}

void mg_hlgfx_build_textures(void)
{
	static bool built = false;
	if (!built)
	{
		rdata->sprite_batch.image_array = mgfx_create_image_array();
		built = true;
	}

    mgfx_update_image_array(rdata->sprite_batch.image_array, rdata->sprite_batch.images, rdata->sprite_batch.samplers, rdata->sprite_batch.image_count);
}

void mg_hlgfx_destroy_textures(void)
{
    mgfx_wait();
    for (uint8_t i = 0; i < rdata->sprite_batch.image_count - 1; i++)
    {
        mgfx_destroy_image(rdata->sprite_batch.images[i]);
        mgfx_destroy_sampler(rdata->sprite_batch.samplers[i]);
    }
    rdata->sprite_batch.image_count = 0;
}