#include "magma/core/event.h"
#include "magma/core/event_types.h"

#include "magma/platform/platform.h"

#include "magma/rendering/rhi_renderer.h"

#include "magma/math/mat4.h"
#include "magma/math/math.h"

#include "magma/rendering/vulkan/shaders/hello_triangle/vert.spv.h"
#include "magma/rendering/vulkan/shaders/hello_triangle/frag.spv.h"

#include "magma/rendering/vulkan/shaders/post_process/vert.spv.h"
#include "magma/rendering/vulkan/shaders/post_process/frag.spv.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static bool is_running = true;

bool is_rendering = true;

mg_image_t frame_texture_image;
mg_framebuffer_t frame_framebuffer;
mg_sampler_t sampler;
mg_render_pass_t back_render_pass;
mg_vec2i_t viewport;
float view_size = 20.0f;

void on_application_quit(mg_application_quit_event_data_t *data)
{
    is_running = false;
}

void on_resize(mg_resized_event_data_t *data)
{
    viewport = (mg_vec2i_t) { data->width, data->height };

    if (data->width == 0 || data->height == 0)
        return;

        mg_rhi_renderer_destroy_framebuffer(frame_framebuffer);
        mg_rhi_renderer_destroy_image(frame_texture_image);

        mg_image_create_info_t texture_image_create_info = { 0 };
        texture_image_create_info.extent.x = data->width;
        texture_image_create_info.extent.y = data->height;
        texture_image_create_info.format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB;
        texture_image_create_info.type = MG_IMAGE_TYPE_2D;

        frame_texture_image = mg_rhi_renderer_create_image(&texture_image_create_info);
        mg_rhi_renderer_update_image(frame_texture_image, sampler);

        mg_framebuffer_create_info_t frame_framebuffer_create_info;
        frame_framebuffer_create_info.image = frame_texture_image;
        frame_framebuffer_create_info.extent = texture_image_create_info.extent;
        frame_framebuffer_create_info.render_pass = back_render_pass;

        frame_framebuffer = mg_rhi_renderer_create_framebuffer(&frame_framebuffer_create_info);

        mg_swapchain_config_info_t config_info = {
            .extent = (mg_vec2i_t) { data->width, data->height },
            .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
            .present_mode = MG_PRESENT_MODE_IMMEDIATE_KHR
        };

        mg_rhi_renderer_configure_swapchain(&config_info);
}

void on_key(mg_key_event_data_t *data)
{
    if (data->key == MG_KEY_E)
        viewport = (mg_vec2i_t){0, 0};
}

void on_mouse_wheel(mg_mouse_wheel_event_data_t *data)
{
    view_size -= data->z_delta * 4.0f;
}

typedef struct UniformBufferObject
{
    mg_mat4_t projection;
}
UniformBufferObject;

typedef struct PushConstantObject
{
    mg_mat4_t model;
}
PushConstantObject;

int main(void)
{
    mg_platform_init_info_t platform_init_info = {
        .title = "Magma",
        .width = 1280,
        .height = 720,
        .position_x = 200,
        .position_y = 100
    };

    mg_event_register(MG_EVENT_CODE_APPLICATION_QUIT, (PFN_on_event)on_application_quit);
    mg_event_register(MG_EVENT_CODE_KEY_PRESSED, on_key);
    mg_event_register(MG_EVENT_CODE_MOUSE_WHEEL, on_mouse_wheel);

    mg_platform_t *platform = mg_platform_initialize(&platform_init_info);
    mg_event_register(MG_EVENT_CODE_RESIZED, (PFN_on_event)on_resize);
    
    mg_renderer_init_info_t renderer_init_info = {
        .platform = platform,
        .type = MG_RENDERER_TYPE_VULKAN,
        .swapchain_config_info = &(mg_swapchain_config_info_t) {
            .extent = (mg_vec2i_t) { platform_init_info.width, platform_init_info.height },
            .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
            .present_mode = MG_PRESENT_MODE_IMMEDIATE_KHR
        }
    };

    mg_rhi_renderer_initialize(&renderer_init_info);

    mg_render_pass_create_info_t render_pass_create_info = {
        .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB
    };

    back_render_pass = mg_rhi_renderer_create_render_pass(&render_pass_create_info);
    mg_render_pass_t render_pass = mg_rhi_renderer_create_render_pass(&render_pass_create_info);

    const float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f,
    };

    const float frame_vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };

    const uint32_t indices[] = {
        0, 1, 3, 1, 2, 3
    };

    mg_vertex_buffer_t frame_vertex_buffer =
        mg_rhi_renderer_create_vertex_buffer(sizeof(frame_vertices), frame_vertices);

    mg_vertex_buffer_t vertex_buffer =
        mg_rhi_renderer_create_vertex_buffer(sizeof(vertices), vertices);

    mg_index_buffer_t index_buffer =
        mg_rhi_renderer_create_index_buffer(sizeof(indices), indices);

    mg_uniform_buffer_t uniform_buffer =
        mg_rhi_renderer_create_uniform_buffer(sizeof(UniformBufferObject));
    
    UniformBufferObject ubo = { 0 };

    mg_shader_source_t vertex_shader;
    vertex_shader.code = VULKAN_HELLO_TRIANGLE_VERT;
    vertex_shader.code_size = sizeof(VULKAN_HELLO_TRIANGLE_VERT);

    mg_shader_source_t fragment_shader;
    fragment_shader.code = VULKAN_HELLO_TRIANGLE_FRAG;
    fragment_shader.code_size = sizeof(VULKAN_HELLO_TRIANGLE_FRAG);

    mg_shader_source_t post_process_vertex_shader;
    post_process_vertex_shader.code = VULKAN_POST_PROCESS_VERT;
    post_process_vertex_shader.code_size = sizeof(VULKAN_POST_PROCESS_VERT);

    mg_shader_source_t post_process_fragment_shader;
    post_process_fragment_shader.code = VULKAN_POST_PROCESS_FRAG;
    post_process_fragment_shader.code_size = sizeof(VULKAN_POST_PROCESS_FRAG);

    mg_vertex_attribute_info_t vertex_attributes[2];
    vertex_attributes[0].location = 0;
    vertex_attributes[0].offset = 0;
    vertex_attributes[0].format = MG_VERTEX_FORMAT_FLOAT2;
    vertex_attributes[1].location = 1;
    vertex_attributes[1].offset = 2 * sizeof(float);
    vertex_attributes[1].format = MG_VERTEX_FORMAT_FLOAT2;

    mg_vertex_layout_info_t vertex_layout;
    vertex_layout.stride = 4 * sizeof(float);
    vertex_layout.attributes = vertex_attributes;
    vertex_layout.attribute_count = 2;

    mg_pipeline_create_info_t pipeline_create_info = { 0 };

    pipeline_create_info.vertex_shader = &vertex_shader;
    pipeline_create_info.fragment_shader = &fragment_shader;

    pipeline_create_info.vertex_layout = vertex_layout;

    pipeline_create_info.polygon_mode = MG_POLYGON_MODE_FILL;
    pipeline_create_info.cull_mode = MG_CULL_MODE_BACK;
    pipeline_create_info.front_face = MG_FRONT_FACE_CW;

    mg_color_blend_t color_blend;
    color_blend.blend_enabled = true;
    color_blend.src_color_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA;
    color_blend.dst_color_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend.color_blend_op = MG_BLEND_OP_ADD;
    color_blend.src_alpha_blend_factor = MG_BLEND_FACTOR_ONE;
    color_blend.dst_alpha_blend_factor = MG_BLEND_FACTOR_ZERO;
    color_blend.alpha_blend_op = MG_BLEND_OP_ADD;

    pipeline_create_info.color_blend = color_blend;
    
    pipeline_create_info.has_push_constants = true;
    pipeline_create_info.push_constants_size = sizeof(PushConstantObject);

    pipeline_create_info.render_pass = render_pass;

    mg_pipeline_t pipeline = mg_rhi_renderer_create_pipeline(&pipeline_create_info);

    pipeline_create_info.vertex_shader = &post_process_vertex_shader;
    pipeline_create_info.fragment_shader = &post_process_fragment_shader;

    color_blend.blend_enabled = false;
    color_blend.src_color_blend_factor = MG_BLEND_FACTOR_ONE;
    color_blend.dst_color_blend_factor = MG_BLEND_FACTOR_ZERO;
    color_blend.color_blend_op = MG_BLEND_OP_ADD;
    color_blend.src_alpha_blend_factor = MG_BLEND_FACTOR_ONE;
    color_blend.dst_alpha_blend_factor = MG_BLEND_FACTOR_ZERO;
    color_blend.alpha_blend_op = MG_BLEND_OP_ADD;

    pipeline_create_info.color_blend = color_blend;

    pipeline_create_info.render_pass = back_render_pass;

    mg_pipeline_t back_pipeline = mg_rhi_renderer_create_pipeline(&pipeline_create_info);

    mg_image_create_info_t texture_image_create_info = { 0 };
    int texWidth, texHeight, texChannels;
    uint8_t *pixels = stbi_load("test.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    texture_image_create_info.extent.x = texWidth;
    texture_image_create_info.extent.y = texHeight;
    texture_image_create_info.format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB;
    texture_image_create_info.type = MG_IMAGE_TYPE_2D;

    mg_image_t texture_image =
        mg_rhi_renderer_create_image(&texture_image_create_info);

    mg_image_write_info_t texture_image_write_info;
    texture_image_write_info.extent = texture_image_create_info.extent;
    texture_image_write_info.data = pixels;
    
    mg_rhi_renderer_write_image(texture_image, &texture_image_write_info);

    stbi_image_free(pixels);
    
    viewport = (mg_vec2i_t) { 1280, 720 };

    texture_image_create_info.extent = viewport;
    
    frame_texture_image =
        mg_rhi_renderer_create_image(&texture_image_create_info);
    
    mg_framebuffer_create_info_t frame_framebuffer_create_info;
    frame_framebuffer_create_info.image = frame_texture_image;
    frame_framebuffer_create_info.extent = texture_image_create_info.extent;
    frame_framebuffer_create_info.render_pass = back_render_pass;
    
    frame_framebuffer =
        mg_rhi_renderer_create_framebuffer(&frame_framebuffer_create_info);

    mg_sampler_create_info_t sampler_create_info = { 0 };
    sampler_create_info.mag_filter = MG_SAMPLER_FILTER_NEAREST;
    sampler_create_info.min_filter = MG_SAMPLER_FILTER_NEAREST;
    sampler_create_info.address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info.address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info.address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    sampler =
        mg_rhi_renderer_create_sampler(&sampler_create_info);

    mg_rhi_renderer_update_image(texture_image, sampler);
    mg_rhi_renderer_update_image(frame_texture_image, sampler);

    float x = 0;
    float y = 0;
    float current_x = 0;
    float current_y = 0;

    double current_time, last_time = 0;

    float current_view_size = 20.0f;

    while (is_running)
    {
        current_time = mg_platform_get_time();
        const double delta_time = current_time - last_time;
        last_time = current_time;

            mg_rhi_renderer_begin_frame();

            mg_render_pass_begin_info_t render_pass_begin_info;
            render_pass_begin_info.framebuffer = frame_framebuffer;
            render_pass_begin_info.render_area = (mg_vec4_t){0.0f, 0.0f, viewport.x, viewport.y};
            render_pass_begin_info.clear_value = (mg_vec4_t){3.5f / 256.0f, 3.5f / 256.0f, 3.8f / 256.0f, 1.0f};
            mg_rhi_renderer_begin_render_pass(back_render_pass, &render_pass_begin_info);
            if (mg_input_is_key_down(MG_KEY_Z))
                mg_rhi_renderer_viewport(100, 100);
            else
                mg_rhi_renderer_viewport(viewport.x, viewport.y);

            int32_t width, height;
            mg_platform_get_window_size(platform, &width, &height);

            if (height > 0)
            {
                x+=(mg_input_is_key_down(MG_KEY_D) - mg_input_is_key_down(MG_KEY_A)) * (float)height / (float)width * 2.0f * delta_time * current_view_size;
                y+=(mg_input_is_key_down(MG_KEY_S) - mg_input_is_key_down(MG_KEY_W)) * (float)width / (float)width  * 2.0f * delta_time * current_view_size;
                current_x = mg_math_lerp(current_x, x, 20.0f * delta_time);
                current_y = mg_math_lerp(current_y, y, 20.0f * delta_time);
                current_view_size = mg_math_lerp(current_view_size, view_size, 20.0f * delta_time);

                const float aspect_ratio = (float)width / (float)height;
                if (aspect_ratio > 1.0f)
                    ubo.projection = mg_mat4_ortho((float)height / height, (float)-height / height, -(float)width / height, (float)width / height, -1.0f, 1.0f);
                else
                    ubo.projection = mg_mat4_ortho((float)height / width, (float)-height / width , -(float)width / width, (float)width / width, -1.0f, 1.0f);

                ubo.projection = mg_mat4_translate(ubo.projection, (mg_vec3_t){-current_x, -current_y, 0.0f});
                ubo.projection = mg_mat4_scale(ubo.projection, (mg_vec3_t){1.0f / current_view_size, 1.0f / current_view_size, 1.0f});

            }

            mg_rhi_renderer_update_uniform_buffer(uniform_buffer, sizeof(UniformBufferObject), &ubo);
            mg_rhi_renderer_update_uniform_buffer(uniform_buffer, sizeof(UniformBufferObject), &ubo);

            mg_rhi_renderer_bind_pipeline(pipeline);
            mg_rhi_renderer_bind_vertex_buffer(vertex_buffer);
            mg_rhi_renderer_bind_index_buffer(index_buffer, MG_INDEX_TYPE_UINT32);
            mg_rhi_renderer_bind_uniform_buffer(uniform_buffer, pipeline);
            mg_rhi_renderer_bind_image(texture_image, pipeline);

            PushConstantObject push;
            push.model = mg_mat4_identity();
            push.model = mg_mat4_scale(push.model, (mg_vec3_t){texWidth * 0.05f, texHeight * 0.05f, 1.0f});
            mg_rhi_renderer_push_constants(pipeline, sizeof(PushConstantObject), &push);
            mg_rhi_renderer_draw_indexed(6, 0);

            mg_rhi_renderer_end_render_pass();

            render_pass_begin_info.render_area = (mg_vec4_t){0.0f, 0.0f, viewport.x, viewport.y};
            render_pass_begin_info.clear_value = (mg_vec4_t){0.0f, 0.0f, 0.0f, 1.0f};
            mg_rhi_renderer_begin_default_render_pass(&render_pass_begin_info);
            mg_rhi_renderer_viewport(viewport.x, viewport.y);
            //ubo.projection = mg_mat4_identity();

            mg_rhi_renderer_update_uniform_buffer(uniform_buffer, sizeof(UniformBufferObject), &ubo);

            //mg_rhi_renderer_update_descriptor_set(ubo_set, &descriptor_write);

            mg_rhi_renderer_bind_pipeline(back_pipeline);
            mg_rhi_renderer_bind_vertex_buffer(frame_vertex_buffer);
            mg_rhi_renderer_bind_index_buffer(index_buffer, MG_INDEX_TYPE_UINT32);
            //mg_rhi_renderer_bind_descriptor_set(ubo_set, back_pipeline, 0);
            mg_rhi_renderer_bind_uniform_buffer(uniform_buffer, back_pipeline);
            //mg_rhi_renderer_bind_descriptor_set(frame_sampler_set, back_pipeline, 1);
            mg_rhi_renderer_bind_image(frame_texture_image, back_pipeline);
            mg_rhi_renderer_draw_indexed(6, 0);

            mg_rhi_renderer_end_render_pass();

            mg_rhi_renderer_end_frame();
            mg_rhi_renderer_present_frame();
        
        mg_platform_poll_messages(platform);
    }

    mg_rhi_renderer_destroy_pipeline(pipeline);
    mg_rhi_renderer_destroy_pipeline(back_pipeline);
    mg_rhi_renderer_destroy_sampler(sampler);
    mg_rhi_renderer_destroy_image(frame_texture_image);
    mg_rhi_renderer_destroy_image(texture_image);
    mg_rhi_renderer_destroy_uniform_buffer(uniform_buffer);
    mg_rhi_renderer_destroy_index_buffer(index_buffer);
    mg_rhi_renderer_destroy_vertex_buffer(vertex_buffer);
    mg_rhi_renderer_destroy_vertex_buffer(frame_vertex_buffer);
    mg_rhi_renderer_destroy_framebuffer(frame_framebuffer);
    mg_rhi_renderer_destroy_render_pass(render_pass);
    mg_rhi_renderer_destroy_render_pass(back_render_pass);

    mg_rhi_renderer_shutdown();
    mg_platform_shutdown(platform);

    return 0;
}