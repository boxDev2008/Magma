#include "hlrenderer.h"

#include <magma/math/mat4.h>
#include <magma/math/math.h>

#include "shaders/post_process.h"
#include "shaders/sprite.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MG_HLGFX_2D_MAX_GEOMETRY_COUNT (1 << 14)
#define MG_HLGFX_2D_MAX_LIGHT_COUNT 16

typedef struct mg_world_ubo_2d
{
    mg_vec4 light_transform[MG_HLGFX_2D_MAX_LIGHT_COUNT];
    mg_vec4 light_colors[MG_HLGFX_2D_MAX_LIGHT_COUNT];
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
	struct
	{
    	mg_sampler smp;
		mg_image color_img;
		mg_image depth_img;
		mg_image_array img_array;
		mg_framebuffer fb;
		mg_render_pass rp;
		mg_pipeline pip;
	}
	screen_data;
    
    struct
    {
        mg_pipeline pipeline;
        mg_dynamic_vertex_buffer vb;
        mg_index_buffer ib;
        
        mg_batch_quad quads[MG_HLGFX_2D_MAX_GEOMETRY_COUNT];
        uint32_t quad_count;
        uint32_t current_offset;

        mg_image images[MG_CONFIG_MAX_BINDABLE_IMAGES];
        mg_sampler samplers[MG_CONFIG_MAX_BINDABLE_IMAGES];
        mg_image_array image_array;
        uint8_t image_count;
    }
    sprite_batch;

    mg_renderer_type renderer_type;
    int32_t width, height;
}
mg_hlgfx_data;

static mg_hlgfx_data *rdata;

void mg_hlgfx_submit_sprite_batch(void)
{
    mgfx_update_dynamic_vertex_buffer(rdata->sprite_batch.vb,
        rdata->sprite_batch.quad_count * sizeof(mg_batch_quad), rdata->sprite_batch.quads);
    mgfx_bind_dynamic_vertex_buffer(rdata->sprite_batch.vb);
    mgfx_bind_index_buffer(rdata->sprite_batch.ib, MG_INDEX_TYPE_UINT32);
	if (rdata->sprite_batch.image_array)
    	mgfx_bind_image_array(rdata->sprite_batch.image_array);

    uint32_t offset = rdata->sprite_batch.quad_count * 6;

    mgfx_draw_indexed(offset - rdata->sprite_batch.current_offset, rdata->sprite_batch.current_offset, 0);
    rdata->sprite_batch.current_offset = offset;
}

void mg_hlgfx_begin_scene_2d(mg_camera_info_2d *camera_info)
{
	const int32_t width = rdata->width;
    const int32_t height = rdata->height;

	struct
	{
		mg_mat4 vp;
	}
	ub_data;
    
    ub_data.vp = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);
	ub_data.vp = mg_mat4_translate(ub_data.vp, (mg_vec3) { -camera_info->position.x, -camera_info->position.y, 0.0f });
	ub_data.vp = mg_mat4_scale(ub_data.vp, (mg_vec3) { camera_info->zoom.x, camera_info->zoom.y, 1.0f });
	mgfx_bind_pipeline(rdata->sprite_batch.pipeline);
	mgfx_bind_uniforms(0, sizeof(ub_data), &ub_data);
}

void mg_hlgfx_end_scene_2d(void)
{
	mg_hlgfx_submit_sprite_batch();
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
    
#pragma region SCREEN_DATA
    {
        mg_sampler_create_info sampler_create_info = {
            .mag_filter = MG_SAMPLER_FILTER_NEAREST,
            .min_filter = MG_SAMPLER_FILTER_NEAREST,
            .address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
        };
        
        rdata->screen_data.smp =
            mgfx_create_sampler(&sampler_create_info);

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
		
		rdata->screen_data.color_img =
			mgfx_create_image(&color_attachment_info);
	
		rdata->screen_data.img_array = mgfx_create_image_array();
		mgfx_update_image_array(
			rdata->screen_data.img_array,
			&rdata->screen_data.color_img,
			&rdata->screen_data.smp,
			1
		);
		
		render_pass_info.depth_stencil_attachment = (mg_attachment_info){
			.format = MG_PIXEL_FORMAT_D24_UNORM_S8_UINT
		};
		render_pass_info.has_depth_stencil_attachment = true;
		
		mg_image_create_info depth_stencil_attachment_create_info = {
			.format = MG_PIXEL_FORMAT_D24_UNORM_S8_UINT,
			.type = MG_IMAGE_TYPE_2D,
			.usage = MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT,
			.width = rdata->width,
			.height = rdata->height
		};
		
		rdata->screen_data.depth_img =
			mgfx_create_image(&depth_stencil_attachment_create_info);
		
		rdata->screen_data.rp =
			mgfx_create_render_pass(&render_pass_info);
		
		mg_framebuffer_create_info framebuffer_info = {
			.width = rdata->width,
			.height = rdata->height,
			.color_attachment = rdata->screen_data.color_img,
			.depth_stencil_attachment = rdata->screen_data.depth_img,
			.render_pass = rdata->screen_data.rp
		};
		
		rdata->screen_data.fb =
			mgfx_create_framebuffer(&framebuffer_info);
		
		mg_pipeline_create_info pipeline_create_info = {
			.shader = get_post_process_shader(rdata->renderer_type),
	
			.primitive_topology = MG_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
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
			}
		};
		
		rdata->screen_data.pip =
			mgfx_create_pipeline(&pipeline_create_info);
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
			
			.render_pass = rdata->screen_data.rp
        };
        
        rdata->sprite_batch.pipeline =
            mgfx_create_pipeline(&pipeline_create_info);

        rdata->sprite_batch.vb =
            mgfx_create_dynamic_vertex_buffer(MG_HLGFX_2D_MAX_GEOMETRY_COUNT * sizeof(mg_batch_quad));
        
        uint32_t indices[MG_HLGFX_2D_MAX_GEOMETRY_COUNT * 6];
        
        for (uint32_t i = 0; i < MG_HLGFX_2D_MAX_GEOMETRY_COUNT; i++)
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
    
    mgfx_destroy_dynamic_vertex_buffer(rdata->sprite_batch.vb);
    mgfx_destroy_index_buffer(rdata->sprite_batch.ib);
    mgfx_destroy_pipeline(rdata->sprite_batch.pipeline);

	if (rdata->sprite_batch.image_array)
    	mgfx_destroy_image_array(rdata->sprite_batch.image_array);

	mgfx_destroy_framebuffer(rdata->screen_data.fb);
	mgfx_destroy_image(rdata->screen_data.depth_img);
	mgfx_destroy_image(rdata->screen_data.color_img);
	mgfx_destroy_image_array(rdata->screen_data.img_array);
	mgfx_destroy_pipeline(rdata->screen_data.pip);
	mgfx_destroy_render_pass(rdata->screen_data.rp);
    mgfx_destroy_sampler(rdata->screen_data.smp);

    mgfx_shutdown();

    free(rdata);
}

void mg_hlgfx_resize(int32_t width, int32_t height)
{
    rdata->width = width;
    rdata->height = height;

    mg_swapchain_config_info config_info;
	config_info.format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB;
	config_info.present_mode = MG_PRESENT_MODE_FIFO;
	config_info.width = width;
	config_info.height = height;

	mgfx_destroy_framebuffer(rdata->screen_data.fb);
    mgfx_destroy_image(rdata->screen_data.color_img);
	mgfx_destroy_image(rdata->screen_data.depth_img);

    mg_image_create_info color_attachment_info = {
        .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
        .type = MG_IMAGE_TYPE_2D,
        .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
        .width = width,
        .height = height
    };
    
    rdata->screen_data.color_img =
        mgfx_create_image(&color_attachment_info);

    mgfx_update_image_array(rdata->screen_data.img_array, &rdata->screen_data.color_img, &rdata->screen_data.smp, 1);

    mg_framebuffer_create_info framebuffer_create_info = {
        .width = width,
        .height = height,
        .render_pass = rdata->screen_data.rp,
        .color_attachment = rdata->screen_data.color_img,
    };
    
	mg_image_create_info depth_stencil_attachment_create_info = {
		.format = MG_PIXEL_FORMAT_D24_UNORM_S8_UINT,
		.type = MG_IMAGE_TYPE_2D,
		.usage = MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT,
		.width = width,
		.height = height
	};
	
	framebuffer_create_info.depth_stencil_attachment = rdata->screen_data.depth_img =
		mgfx_create_image(&depth_stencil_attachment_create_info);
    
    rdata->screen_data.fb =
        mgfx_create_framebuffer(&framebuffer_create_info);

    mgfx_configure_swapchain(&config_info);        
}

void mg_hlgfx_begin(void)
{
    rdata->sprite_batch.quad_count = 0;
    rdata->sprite_batch.current_offset = 0;

    mgfx_begin();

	mg_render_pass_begin_info rp_info = {
        .render_area = (mg_vec4) {0.0f, 0.0f, rdata->width, rdata->height},
        .clear_value = (mg_vec4) {0.0f, 0.0f, 0.0f, 1.0f}
    };
	mgfx_begin_render_pass(rdata->screen_data.rp, rdata->screen_data.fb, &rp_info);
	mgfx_viewport(0, 0, rdata->width, rdata->height);
}

void mg_hlgfx_end(void)
{
	mgfx_end_render_pass();

    mg_render_pass_begin_info rp_info = {
        .render_area = (mg_vec4) {0.0f, 0.0f, rdata->width, rdata->height},
        .clear_value = (mg_vec4) {0.0f, 0.0f, 0.0f, 1.0f}
    };
    
    mgfx_begin_default_render_pass(&rp_info);
    mgfx_viewport(0, 0, rdata->width, rdata->height);

	mgfx_bind_pipeline(rdata->screen_data.pip);
	mgfx_bind_image_array(rdata->screen_data.img_array);
	mgfx_draw(4, 0);
    
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

/*void mg_hlgfx_draw_point_light_2d(mg_vec2 position, float scale, float intensity, mg_vec3 color)
{
    if (rdata->world_ubo_2d.light_count >= MG_HLGFX_2D_MAX_LIGHT_COUNT)
        rdata->world_ubo_2d.light_count = 0;
    
    if (rdata->renderer_type == MG_RENDERER_TYPE_DIRECT3D11)
        position.y = position.y * -1.0f + 1.0f;
    
    rdata->world_ubo_2d.light_transform[rdata->world_ubo_2d.light_count] =
    (mg_vec4) { position.x, position.y, scale, intensity };
    rdata->world_ubo_2d.light_colors[rdata->world_ubo_2d.light_count] =
    (mg_vec4) { color.r, color.g, color.b, 1.0f };
    
    rdata->world_ubo_2d.light_count++;
}*/

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
		return (mg_texture) { 0 };
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