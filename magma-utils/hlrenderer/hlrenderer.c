#include "hlrenderer.h"

#include <magma/math/mat4.h>
#include <magma/math/math.h>

#include "shaders/post_process.h"
#include "shaders/sprite.h"
#include "shaders/lit_sprite.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef MG_HLGFX_2D_MAX_GEOMETRY_COUNT
    #define MG_HLGFX_2D_MAX_GEOMETRY_COUNT (1 << 14)
#endif

#define MG_HLGFX_2D_MAX_LIGHT_COUNT 128

typedef struct mg_batch_vertex
{
    mg_vec2 position;
    mg_vec2 texcoord;
    uint32_t color;
    float tex_id;
    float grayscale;
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
		mg_render_pass rp;
		mg_pipeline pip;
	}
	screen_data;

    struct
	{
        mg_vec4 light_transform[MG_HLGFX_2D_MAX_LIGHT_COUNT];
        mg_vec4 light_colors[MG_HLGFX_2D_MAX_LIGHT_COUNT];
        mg_vec4 global_light_data;
        mg_vec4 resolution;
        int32_t light_count;
	}
	lit_ub_data;

    struct
    {
        mg_vec4 vignette_data;
        int32_t enable_vignette;
    }
    post_process_ub_data;
    
    struct
    {
        mg_pipeline pipeline;
        mg_pipeline lit_pipeline;

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

void mg_hlgfx_begin_scene_2d(const mg_camera_info_2d *camera_info)
{
	const int32_t width = rdata->width;
    const int32_t height = rdata->height;

	struct
	{
		mg_mat4 vp;
	}
	ub_data;
    
    const float half_width = width * 0.5f;
    const float half_height = height * 0.5f;

    ub_data.vp = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);
	ub_data.vp = mg_mat4_translate(ub_data.vp, (mg_vec3) { -camera_info->position.x / half_width, -camera_info->position.y / half_height, 0.0f });
	ub_data.vp = mg_mat4_scale(ub_data.vp, (mg_vec3) { camera_info->zoom.x, camera_info->zoom.y, 1.0f });
	mgfx_bind_pipeline(rdata->sprite_batch.pipeline);
	mgfx_bind_uniforms(0, sizeof(ub_data), &ub_data);
}

void mg_hlgfx_end_scene_2d(void)
{
	mg_hlgfx_submit_sprite_batch();
}

void mg_hlgfx_begin_lit_scene_2d(const mg_camera_info_2d *camera_info, mg_lit_scene_info_2d *lit_scene_info)
{
	const int32_t width = rdata->width;
    const int32_t height = rdata->height;

	struct
	{
		mg_mat4 vp;
	}
	vs_ub_data;

    const float half_width = width * 0.5f;
    const float half_height = height * 0.5f;

    vs_ub_data.vp = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);
	vs_ub_data.vp = mg_mat4_translate(vs_ub_data.vp, (mg_vec3) { -camera_info->position.x / half_width, -camera_info->position.y / half_height, 0.0f });
	vs_ub_data.vp = mg_mat4_scale(vs_ub_data.vp, (mg_vec3) { camera_info->zoom.x, camera_info->zoom.y, 1.0f });
	mgfx_bind_pipeline(rdata->sprite_batch.lit_pipeline);
	mgfx_bind_uniforms(0, sizeof(vs_ub_data), &vs_ub_data);

    rdata->lit_ub_data.light_count = 0;
    rdata->lit_ub_data.resolution = (mg_vec4) { (float)width, (float)height, 0.0f, 0.0f };
    rdata->lit_ub_data.global_light_data = (mg_vec4) {
        lit_scene_info->global_light_color.r,
        lit_scene_info->global_light_color.g,
        lit_scene_info->global_light_color.b,
        lit_scene_info->global_light_intensity
    };
}

void mg_hlgfx_end_lit_scene_2d(void)
{
    mgfx_bind_uniforms(1, sizeof(rdata->lit_ub_data), &rdata->lit_ub_data);
    mg_hlgfx_submit_sprite_batch();
}

void mg_hlgfx_initialize(const mg_hlgfx_init_info *info)
{
    rdata = (mg_hlgfx_data*)calloc(1, sizeof(mg_hlgfx_data));

    mg_swapchain_config_info swapchain_info;
    swapchain_info.format = MG_PIXEL_FORMAT_B8G8R8A8_UNORM;
	swapchain_info.vsync = true;
    swapchain_info.width = info->width;
    swapchain_info.height = info->height;

    mgfx_init_info renderer_info;
    renderer_info.type = info->type;
    renderer_info.swapchain_config_info = &swapchain_info;

    mgfx_initialize(&renderer_info);

    rdata->width = info->width;
    rdata->height = info->height;
    rdata->renderer_type = info->type;
    
#pragma region SCREEN_DATA
    {
        mg_sampler_create_info sampler_create_info = {
            .mag_filter = MG_SAMPLER_FILTER_LINEAR,
            .min_filter = MG_SAMPLER_FILTER_LINEAR,
            .address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
        };
        
        rdata->screen_data.smp =
            mgfx_create_sampler(&sampler_create_info);
		
		mg_image_create_info color_attachment_info = {
			.format = MG_PIXEL_FORMAT_R8G8B8A8_UNORM,
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
		
		mg_image_create_info depth_stencil_attachment_create_info = {
			.format = MG_PIXEL_FORMAT_D32_SFLOAT,
			.type = MG_IMAGE_TYPE_2D,
			.usage = MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT,
			.width = rdata->width,
			.height = rdata->height
		};
		
		rdata->screen_data.depth_img =
			mgfx_create_image(&depth_stencil_attachment_create_info);
		
		mg_render_pass_create_info render_pass_info = {
			.color_attachment = {
				.format = MG_PIXEL_FORMAT_R8G8B8A8_UNORM,
                .image = rdata->screen_data.color_img
			},
            .depth_stencil_attachment = {
                .format = MG_PIXEL_FORMAT_D24_UNORM_S8_UINT,
                .image = rdata->screen_data.depth_img
            },
            .width = rdata->width,
            .height = rdata->height
		};
		rdata->screen_data.rp =
			mgfx_create_render_pass(&render_pass_info);

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
        mg_vertex_layout_info vertex_layout = {
            .stride = 7 * sizeof(float),
            .attribute_count = 5,
            .attributes = {
                { .location = 0, .offset = 0, .format = MG_VERTEX_FORMAT_FLOAT2 },
                { .location = 1, .offset = 2 * sizeof(float), .format = MG_VERTEX_FORMAT_FLOAT2 },
                { .location = 2, .offset = 4 * sizeof(float), .format = MG_VERTEX_FORMAT_UBYTE4N },
                { .location = 3, .offset = 5 * sizeof(float), .format = MG_VERTEX_FORMAT_FLOAT },
                { .location = 4, .offset = 6 * sizeof(float), .format = MG_VERTEX_FORMAT_FLOAT }
            }
        };
        
        mg_pipeline_create_info pipeline_create_info = {
            .shader = get_sprite_shader(rdata->renderer_type),
            
            .vertex_layout = vertex_layout,

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

        pipeline_create_info.shader = get_lit_sprite_shader(rdata->renderer_type);
        rdata->sprite_batch.lit_pipeline =
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
    mgfx_destroy_dynamic_vertex_buffer(rdata->sprite_batch.vb);
    mgfx_destroy_index_buffer(rdata->sprite_batch.ib);
    mgfx_destroy_pipeline(rdata->sprite_batch.pipeline);
    mgfx_destroy_pipeline(rdata->sprite_batch.lit_pipeline);

	if (rdata->sprite_batch.image_array)
    	mgfx_destroy_image_array(rdata->sprite_batch.image_array);

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
	config_info.format = MG_PIXEL_FORMAT_B8G8R8A8_UNORM;
	config_info.vsync = true;
	config_info.width = width;
	config_info.height = height;

    mgfx_destroy_image(rdata->screen_data.color_img);
	mgfx_destroy_image(rdata->screen_data.depth_img);

    mg_image_create_info color_attachment_info = {
        .format = MG_PIXEL_FORMAT_R8G8B8A8_UNORM,
        .type = MG_IMAGE_TYPE_2D,
        .usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT,
        .width = width,
        .height = height
    };
    
    rdata->screen_data.color_img =
        mgfx_create_image(&color_attachment_info);

    mgfx_update_image_array(rdata->screen_data.img_array, &rdata->screen_data.color_img, &rdata->screen_data.smp, 1);

	mg_image_create_info depth_stencil_attachment_create_info = {
		.format = MG_PIXEL_FORMAT_D32_SFLOAT,
		.type = MG_IMAGE_TYPE_2D,
		.usage = MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT,
		.width = width,
		.height = height
	};
	
	rdata->screen_data.depth_img =
		mgfx_create_image(&depth_stencil_attachment_create_info);

    mg_render_pass_update_info resize_info = {
        .color_image = rdata->screen_data.color_img,
        .depth_stencil_image = rdata->screen_data.depth_img,
        .width = width,
        .height = height
    };
    mgfx_update_render_pass(rdata->screen_data.rp, &resize_info);

    mgfx_configure_swapchain(&config_info);        
}

void mg_hlgfx_begin(const mg_post_process_info *post_process_info, const mg_vec3 clear_color)
{
    rdata->sprite_batch.quad_count = 0;
    rdata->sprite_batch.current_offset = 0;

    mgfx_begin();

	mg_render_pass_begin_info rp_info = {
        .render_area = (mg_vec4i) {0, 0, rdata->width, rdata->height},
        .clear_value = (mg_vec4) {clear_color.x, clear_color.y, clear_color.z, 1.0f}
    };
	mgfx_begin_render_pass(rdata->screen_data.rp, &rp_info);
	mgfx_viewport(0, 0, rdata->width, rdata->height);

    rdata->post_process_ub_data.vignette_data = (mg_vec4) {
        post_process_info->vignette_color.x,
        post_process_info->vignette_color.y,
        post_process_info->vignette_color.z,
        post_process_info->vignette_strength
    };
    rdata->post_process_ub_data.enable_vignette = post_process_info->flags & MG_POST_PROCESS_FLAG_VIGNETTE;
}

void mg_hlgfx_end(void)
{
	mgfx_end_render_pass();

    mg_render_pass_begin_info rp_info = {
        .render_area = (mg_vec4i) {0, 0, rdata->width, rdata->height},
        .clear_value = (mg_vec4) {0.0f, 0.0f, 0.0f, 1.0f}
    };
    
    mgfx_begin_default_render_pass(&rp_info);
    mgfx_viewport(0, 0, rdata->width, rdata->height);

	mgfx_bind_pipeline(rdata->screen_data.pip);
	mgfx_bind_image_array(rdata->screen_data.img_array);
    mgfx_bind_uniforms(0, sizeof(rdata->post_process_ub_data), &rdata->post_process_ub_data);
	mgfx_draw(4, 0);
    
    mgfx_end_render_pass();
    mgfx_end();
}

static uint32_t mg_color_to_uint32(mg_vec4 v)
{
    v.x = mg_math_clampf(v.x * 255.0f, 0.0f, 255.0f);
    v.y = mg_math_clampf(v.y * 255.0f, 0.0f, 255.0f);
    v.z = mg_math_clampf(v.z * 255.0f, 0.0f, 255.0f);
    v.w = mg_math_clampf(v.w * 255.0f, 0.0f, 255.0f);
    return (uint32_t) v.x | ((uint32_t) v.y << 8) | ((uint32_t) v.z << 16) | ((uint32_t) v.w << 24);
}

void mg_hlgfx_draw_rect_2d_internal(mg_mat4 model, mg_vec4 color)
{
    const mg_vec4 v1 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v2 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v3 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 1.0f, 0.0f, 1.0f });
    const mg_vec4 v4 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 1.0f, 0.0f, 1.0f });

    const uint32_t ncolor = mg_color_to_uint32(color);

    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {v1.x, v1.y, 0.0f, 0.0f, ncolor, -1, -1},
        {v2.x, v2.y, 1.0f, 0.0f, ncolor, -1, -1},
        {v3.x, v3.y, 1.0f, 1.0f, ncolor, -1, -1},
        {v4.x, v4.y, 0.0f, 1.0f, ncolor, -1, -1}
    }};
}

void mg_hlgfx_draw_sprite_2d_internal_ext(mg_mat4 model, mg_vec4 color, float grayscale, const mg_sprite *sprite)
{
    const uint8_t tex_id = sprite->texture->id;

    const mg_vec4 v1 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v2 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v3 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 1.0f, 0.0f, 1.0f });
    const mg_vec4 v4 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 1.0f, 0.0f, 1.0f });

    const uint32_t ncolor = mg_color_to_uint32(color);

    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {v1.x, v1.y, sprite->x, sprite->y, ncolor, tex_id, grayscale},
        {v2.x, v2.y, sprite->x + sprite->width, sprite->y, ncolor, tex_id, grayscale},
        {v3.x, v3.y, sprite->x + sprite->width, sprite->y + sprite->height, ncolor, tex_id, grayscale},
        {v4.x, v4.y, sprite->x, sprite->y + sprite->height, ncolor, tex_id, grayscale}
    }};
}

void mg_hlgfx_draw_sprite_2d_internal(mg_mat4 model, mg_vec4 color, const mg_sprite *sprite)
{
    mg_hlgfx_draw_sprite_2d_internal_ext(model, color, 0.0f, sprite);
}

void mg_hlgfx_draw_rect_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color)
{
    const mg_vec2 pivot_offset = (mg_vec2) { pivot.x * scale.x, pivot.y * scale.y };
    const uint32_t ncolor = mg_color_to_uint32(color);

    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {position.x - pivot_offset.x, position.y - pivot_offset.y, 0.0f, 0.0f, ncolor, -1, -1},
        {position.x + scale.x - pivot_offset.x, position.y - pivot_offset.y, 0.1f, 0.0f, ncolor, -1, -1},
        {position.x + scale.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, 1.0f, 1.0f, ncolor, -1, -1},
        {position.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, 0.0f, 1.0f, ncolor, -1, -1}
    }};
}

void mg_hlgfx_draw_sprite_2d_ext(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, float grayscale, const mg_sprite *sprite)
{
    const mg_vec2 pivot_offset = (mg_vec2) { pivot.x * scale.x, pivot.y * scale.y };
    const uint32_t ncolor = mg_color_to_uint32(color);

    const uint8_t tex_id = sprite->texture->id;
    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {position.x - pivot_offset.x, position.y - pivot_offset.y, sprite->x, sprite->y, ncolor, tex_id, grayscale},
        {position.x + scale.x - pivot_offset.x, position.y - pivot_offset.y, sprite->x + sprite->width, sprite->y, ncolor, tex_id, grayscale},
        {position.x + scale.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, sprite->x + sprite->width, sprite->y + sprite->height, ncolor, tex_id, grayscale},
        {position.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, sprite->x, sprite->y + sprite->height, ncolor, tex_id, grayscale}
    }};
}


void mg_hlgfx_draw_sprite_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 color, const mg_sprite *sprite)
{
    mg_hlgfx_draw_sprite_2d_ext(position, scale, pivot, color, 0.0f, sprite);
}

float mg_hlgfx_calculate_text_width(float scale, mg_font *font, const char *text)
{
    float text_width = 0.0f;
    const char *p = text;
    while (*p)
    {
        if (*p >= 32 && *p < 128)
        {
            stbtt_bakedchar *b = &font->cdata[*p - 32];
            text_width += b->xadvance * scale;
        }
        ++p;
    }
    return text_width;
}

void mg_hlgfx_draw_text_2d(mg_vec2 position, float scale, mg_vec4 color, mg_text_alignment alignment, mg_font *font, const char *fmt, ...)
{
    char buffer[1024];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    const char *text = buffer;
    const uint8_t tex_id = font->texture.id;
    scale /= 86.0f;

    if (alignment == MG_TEXT_ALIGNMENT_CENTER)
        position.x -= mg_hlgfx_calculate_text_width(scale, font, text) * 0.5f;
    else if (alignment == MG_TEXT_ALIGNMENT_RIGHT)
        position.x -= mg_hlgfx_calculate_text_width(scale, font, text);

    const uint32_t ncolor = mg_color_to_uint32(color);

    while (*text)
    {
        if (*text >= 32 && *text < 128)
        {
            stbtt_bakedchar *b = &font->cdata[*text - 32];

            float x0 = position.x + b->xoff * scale;
            float y0 = position.y + b->yoff * scale;
            float w  = (b->x1 - b->x0) * scale;
            float h  = (b->y1 - b->y0) * scale;

            float x1 = x0 + w;
            float y1 = y0 + h;

            float s0 = (b->x0 + 2) / 2048.0f;
            float t0 = b->y0 / 512.0f;
            float s1 = (b->x1 - 2) / 2048.0f;
            float t1 = b->y1 / 512.0f;

            rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
                {x0, y0, s0, t0, ncolor, tex_id, -1},
                {x1, y0, s1, t0, ncolor, tex_id, -1},
                {x1, y1, s1, t1, ncolor, tex_id, -1},
                {x0, y1, s0, t1, ncolor, tex_id, -1}
            }};

            position.x += b->xadvance * scale;
        }
        ++text;
    }
}

void mg_hlgfx_draw_vertex_colored_sprite_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, mg_vec4 c0 , mg_vec4 c1 , mg_vec4 c2 , mg_vec4 c3, const mg_sprite *sprite)
{
    const mg_vec2 pivot_offset = (mg_vec2) { pivot.x * scale.x, pivot.y * scale.y };
    const uint8_t tex_id = sprite->texture->id;
    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {position.x - pivot_offset.x, position.y - pivot_offset.y, sprite->x, sprite->y, mg_color_to_uint32(c0), tex_id, -1},
        {position.x + scale.x - pivot_offset.x, position.y - pivot_offset.y, sprite->x + sprite->width, mg_color_to_uint32(c1), tex_id, -1},
        {position.x + scale.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, sprite->x + sprite->width, sprite->y + sprite->height, mg_color_to_uint32(c2), tex_id, -1},
        {position.x - pivot_offset.x, position.y + scale.y - pivot_offset.y, sprite->x, sprite->y + sprite->height, mg_color_to_uint32(c3), tex_id, -1}
    }};
}

void mg_hlgfx_draw_rotated_rect_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3){ position.x, position.y, 0.0f });
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_scale(model, (mg_vec3){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3){ -pivot.x, -pivot.y, 0.0f });
    mg_hlgfx_draw_rect_2d_internal(model, color);
}

void mg_hlgfx_draw_rotated_sprite_2d_ext(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, float grayscale, const mg_sprite *sprite)
{
    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3){ position.x, position.y, 0.0f });
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_scale(model, (mg_vec3){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3){ -pivot.x, -pivot.y, 0.0f });
    mg_hlgfx_draw_sprite_2d_internal_ext(model, color, grayscale, sprite);
}

void mg_hlgfx_draw_rotated_sprite_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 color, const mg_sprite *sprite)
{
    mg_hlgfx_draw_rotated_sprite_2d_ext(position, scale, pivot, rotation, color, 0.0f, sprite);
}

void mg_hlgfx_draw_rotated_vertex_colored_sprite_2d(mg_vec2 position, mg_vec2 scale, mg_vec2 pivot, float rotation, mg_vec4 c0, mg_vec4 c1, mg_vec4 c2, mg_vec4 c3, const mg_sprite *sprite)
{
    const uint8_t tex_id = sprite->texture->id;

    mg_mat4 model = mg_mat4_identity();
    model = mg_mat4_translate(model, (mg_vec3){ position.x, position.y, 0.0f });
    model = mg_mat4_rotate_z(model, rotation);
    model = mg_mat4_scale(model, (mg_vec3){ scale.x, scale.y, 1.0f });
    model = mg_mat4_translate(model, (mg_vec3){ -pivot.x, -pivot.y, 0.0f });

    const mg_vec4 v1 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v2 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 0.0f, 0.0f, 1.0f });
    const mg_vec4 v3 = mg_mat4_multiply_vec4(model, (mg_vec4){ 1.0f, 1.0f, 0.0f, 1.0f });
    const mg_vec4 v4 = mg_mat4_multiply_vec4(model, (mg_vec4){ 0.0f, 1.0f, 0.0f, 1.0f });

    rdata->sprite_batch.quads[rdata->sprite_batch.quad_count++] = (mg_batch_quad){{
        {v1.x, v1.y, sprite->x, sprite->y, mg_color_to_uint32(c0), tex_id, -1},
        {v2.x, v2.y, sprite->x + sprite->width, sprite->y, mg_color_to_uint32(c1), tex_id, -1},
        {v3.x, v3.y, sprite->x + sprite->width, sprite->y + sprite->height, mg_color_to_uint32(c2), tex_id, -1},
        {v4.x, v4.y, sprite->x, sprite->y + sprite->height, mg_color_to_uint32(c3), tex_id, -1}
    }};
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
    if (rdata->lit_ub_data.light_count >= MG_HLGFX_2D_MAX_LIGHT_COUNT)
        return;
    
    //if (rdata->renderer_type == MG_RENDERER_TYPE_DIRECT3D11)
        //position.y = position.y * -1.0f + 1.0f;
    
    rdata->lit_ub_data.light_transform[rdata->lit_ub_data.light_count] =
    (mg_vec4) { position.x, position.y, scale, intensity };
    rdata->lit_ub_data.light_colors[rdata->lit_ub_data.light_count] =
    (mg_vec4) { color.r, color.g, color.b, 1.0f };
    
    rdata->lit_ub_data.light_count++;
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
    for (uint8_t i = 0; i < rdata->sprite_batch.image_count - 1; i++)
    {
        mgfx_destroy_image(rdata->sprite_batch.images[i]);
        mgfx_destroy_sampler(rdata->sprite_batch.samplers[i]);
    }
    rdata->sprite_batch.image_count = 0;
}

mg_font mg_hlgfx_add_font(void *ttf_data)
{
    uint32_t *bitmap = (uint32_t*)malloc(512 * 512 * sizeof(uint32_t));
    mg_font font;

    stbtt_BakeFontBitmap(ttf_data, 0, 86.0, (uint8_t*)bitmap, 2048, 256, 32, 96, font.cdata);

    uint32_t* p = bitmap;
    uint32_t* end = bitmap + (512 * 512);
    while (p < end)
    {
        uint32_t a = *p;
        *p++ = (a >> 24 > 200) ? 0xFFFFFFFF : 0x00FFFFFF;
    }

    font.texture = mg_hlgfx_add_texture(512, 512, MG_SAMPLER_FILTER_NEAREST, MG_PIXEL_FORMAT_R8G8B8A8_UNORM, bitmap);
    free(bitmap);
    return font;
}

mg_font mg_hlgfx_add_font_from_file(const char* file_name)
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

    mg_font font = mg_hlgfx_add_font(buffer);

    free(buffer);
    return font;
}