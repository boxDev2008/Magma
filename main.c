#include "magma/core/event.h"
#include "magma/core/event_types.h"

#include "magma/platform/platform.h"

#include "magma/rendering/llapi_renderer.h"

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

mg_texture_image_t frame_texture_image;
mg_texture_view_t frame_texture_view;
mg_framebuffer_t frame_framebuffer;
mg_sampler_t sampler;
mg_descriptor_set_t frame_sampler_set;
mg_render_pass_t back_render_pass;
mg_vec2i_t viewport;
float view_size = 27.0f;

void on_application_quit(mg_application_quit_event_data_t *data)
{
    is_running = false;
}

void on_resize(mg_resized_event_data_t *data)
{
    viewport = (mg_vec2i_t) { data->width, data->height };

    if (data->width == 0 || data->height == 0)
        return;

        mg_llapi_renderer_destroy_framebuffer(frame_framebuffer);
        mg_llapi_renderer_destroy_texture_image(frame_texture_image);
        mg_llapi_renderer_destroy_texture_view(frame_texture_view);

        mg_texture_image_create_info_t texture_image_create_info = { 0 };
        texture_image_create_info.extent.x = data->width;
        texture_image_create_info.extent.y = data->height;

        frame_texture_image = mg_llapi_renderer_create_texture_image(&texture_image_create_info);
        frame_texture_view = mg_llapi_renderer_create_texture_view(frame_texture_image);

        mg_framebuffer_create_info_t frame_framebuffer_create_info;
        frame_framebuffer_create_info.texture_view = frame_texture_view;
        frame_framebuffer_create_info.extent = texture_image_create_info.extent;
        frame_framebuffer_create_info.render_pass = back_render_pass;

        frame_framebuffer = mg_llapi_renderer_create_framebuffer(&frame_framebuffer_create_info);

        mg_descriptor_image_info_t image_info;
        image_info.view = frame_texture_view;
        image_info.sampler = sampler;

        mg_descriptor_write_t descriptor_write = { 0 };
        descriptor_write.binding = 0;
        descriptor_write.descriptor_type = MG_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write.image_info = &image_info;

        mg_llapi_renderer_update_descriptor_set(frame_sampler_set, &descriptor_write);

        mg_swapchain_config_info_t config_info = {
            .extent = (mg_vec2i_t) { data->width, data->height },
            .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
            .present_mode = MG_PRESENT_MODE_IMMEDIATE_KHR
        };

        mg_llapi_renderer_configure_swapchain(&config_info);
}

void on_key(mg_key_event_data_t *data)
{
    if (data->key == MG_KEY_E)
        viewport = (mg_vec2i_t){0, 0};
}

void on_mouse_wheel(mg_mouse_wheel_event_data_t *data)
{
    view_size -= data->z_delta * 8.0f;
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

    mg_llapi_renderer_initialize(&renderer_init_info);

    back_render_pass = mg_llapi_renderer_create_render_pass();
    mg_render_pass_t render_pass = mg_llapi_renderer_create_render_pass();

    const float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f,
        
        0.5f, -0.5f, 0.0f, 0.0f,
        1.5f, -0.5f, 1.0f, 0.0f,
        1.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f
    };

    const float frame_vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };

    const uint32_t indices[] = {
        0, 1, 3, 1, 2, 3,
        4, 5, 7, 5, 6, 7
    };

    mg_buffer_create_info_t frame_vertex_buffer_create_info;
    frame_vertex_buffer_create_info.usage = MG_BUFFER_USAGE_VERTEX;
    frame_vertex_buffer_create_info.size = sizeof(frame_vertices);
    frame_vertex_buffer_create_info.frequency = MG_BUFFER_UPDATE_FREQUENCY_STATIC;
    frame_vertex_buffer_create_info.mapped_at_creation = false;
    mg_buffer_t frame_vertex_buffer =
        mg_llapi_renderer_create_buffer(&frame_vertex_buffer_create_info);

    mg_buffer_create_info_t vertex_buffer_create_info;
    vertex_buffer_create_info.usage = MG_BUFFER_USAGE_VERTEX;
    vertex_buffer_create_info.size = sizeof(vertices);
    vertex_buffer_create_info.frequency = MG_BUFFER_UPDATE_FREQUENCY_STATIC;
    vertex_buffer_create_info.mapped_at_creation = false;
    mg_buffer_t vertex_buffer =
        mg_llapi_renderer_create_buffer(&vertex_buffer_create_info);

    mg_buffer_create_info_t index_buffer_create_info;
    index_buffer_create_info.usage = MG_BUFFER_USAGE_INDEX;
    index_buffer_create_info.size = sizeof(indices);
    index_buffer_create_info.frequency = MG_BUFFER_UPDATE_FREQUENCY_STATIC;
    index_buffer_create_info.mapped_at_creation = false;
    mg_buffer_t index_buffer =
        mg_llapi_renderer_create_buffer(&index_buffer_create_info);

    mg_buffer_create_info_t uniform_buffer_create_info;
    uniform_buffer_create_info.usage = MG_BUFFER_USAGE_UNIFORM;
    uniform_buffer_create_info.size = sizeof(UniformBufferObject);
    uniform_buffer_create_info.frequency = MG_BUFFER_UPDATE_FREQUENCY_DYNAMIC;
    uniform_buffer_create_info.mapped_at_creation = true;
    mg_buffer_t uniform_buffer =
        mg_llapi_renderer_create_buffer(&uniform_buffer_create_info);
    
    mg_buffer_update_info_t frame_vertex_buffer_update_info;
    frame_vertex_buffer_update_info.size = sizeof(frame_vertices);
    frame_vertex_buffer_update_info.data = frame_vertices;
    mg_llapi_renderer_update_buffer(frame_vertex_buffer, &frame_vertex_buffer_update_info);

    mg_buffer_update_info_t vertex_buffer_update_info;
    vertex_buffer_update_info.size = sizeof(vertices);
    vertex_buffer_update_info.data = vertices;
    mg_llapi_renderer_update_buffer(vertex_buffer, &vertex_buffer_update_info);

    mg_buffer_update_info_t index_buffer_update_info;
    index_buffer_update_info.size = sizeof(indices);
    index_buffer_update_info.data = indices;
    mg_llapi_renderer_update_buffer(index_buffer, &index_buffer_update_info);

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

    mg_descriptor_t ubo_descriptor;
    ubo_descriptor.binding = 0;
    ubo_descriptor.stage = MG_SHADER_STAGE_VERTEX;
    ubo_descriptor.type = MG_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    
    mg_descriptor_set_layout_t ubo_layout;
    mg_descriptor_set_layout_create_info_t ubo_layout_create_info;
    ubo_layout_create_info.descriptor_count = 1;
    ubo_layout_create_info.descriptors = &ubo_descriptor;
    ubo_layout = mg_llapi_renderer_create_descriptor_set_layout(&ubo_layout_create_info);

    mg_descriptor_set_t ubo_set;
    mg_descriptor_set_create_info_t ubo_set_create_info;
    ubo_set_create_info.layouts = &ubo_layout;
    ubo_set = mg_llapi_renderer_create_descriptor_set(&ubo_set_create_info);

    mg_descriptor_t sampler_descriptor;
    sampler_descriptor.binding = 0;
    sampler_descriptor.stage = MG_SHADER_STAGE_FRAGMENT;
    sampler_descriptor.type = MG_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    mg_descriptor_set_layout_t sampler_layout;
    mg_descriptor_set_layout_create_info_t sampler_layout_create_info;
    sampler_layout_create_info.descriptor_count = 1;
    sampler_layout_create_info.descriptors = &sampler_descriptor;
    sampler_layout = mg_llapi_renderer_create_descriptor_set_layout(&sampler_layout_create_info);

    mg_pipeline_create_info_t pipeline_create_info = { 0 };

    mg_descriptor_set_layout_t layouts[] = {
        ubo_layout, sampler_layout
    };

    pipeline_create_info.desctriptor_set_layout_count = 2;
    pipeline_create_info.desctriptor_set_layouts = layouts;

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

    mg_pipeline_t pipeline = mg_llapi_renderer_create_pipeline(&pipeline_create_info);

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

    mg_pipeline_t back_pipeline = mg_llapi_renderer_create_pipeline(&pipeline_create_info);

    mg_texture_image_create_info_t texture_image_create_info = { 0 };
    int texWidth, texHeight, texChannels;
    uint8_t *pixels = stbi_load("texture.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    texture_image_create_info.extent.x = texWidth;
    texture_image_create_info.extent.y = texHeight;

    mg_texture_image_t texture_image =
        mg_llapi_renderer_create_texture_image(&texture_image_create_info);

    mg_texture_image_write_info_t texture_image_write_info;
    texture_image_write_info.extent = texture_image_create_info.extent;
    texture_image_write_info.data = pixels;
    
    mg_llapi_renderer_write_texture_image(texture_image, &texture_image_write_info);

    stbi_image_free(pixels);

    mg_texture_view_t texture_view =
        mg_llapi_renderer_create_texture_view(texture_image);
    
    viewport = (mg_vec2i_t) { 1280, 720 };

    texture_image_create_info.extent = viewport;
    
    frame_texture_image =
        mg_llapi_renderer_create_texture_image(&texture_image_create_info);

    frame_texture_view =
        mg_llapi_renderer_create_texture_view(frame_texture_image);
    
    mg_framebuffer_create_info_t frame_framebuffer_create_info;
    frame_framebuffer_create_info.texture_view = frame_texture_view;
    frame_framebuffer_create_info.extent = texture_image_create_info.extent;
    frame_framebuffer_create_info.render_pass = back_render_pass;
    
    frame_framebuffer =
        mg_llapi_renderer_create_framebuffer(&frame_framebuffer_create_info);

    mg_sampler_create_info_t sampler_create_info = { 0 };
    sampler_create_info.mag_filter = MG_SAMPLER_FILTER_NEAREST;
    sampler_create_info.min_filter = MG_SAMPLER_FILTER_NEAREST;
    sampler_create_info.address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info.address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info.address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    sampler =
        mg_llapi_renderer_create_sampler(&sampler_create_info);

    mg_descriptor_set_t sampler_set;
    mg_descriptor_set_create_info_t sampler_set_create_info;
    sampler_set_create_info.layouts = &sampler_layout;
    sampler_set = mg_llapi_renderer_create_descriptor_set(&sampler_set_create_info);

    frame_sampler_set = mg_llapi_renderer_create_descriptor_set(&sampler_set_create_info);

    {
        mg_descriptor_image_info_t image_info;
        image_info.view = texture_view;
        image_info.sampler = sampler;

        mg_descriptor_write_t descriptor_write = { 0 };
        descriptor_write.binding = 0;
        descriptor_write.descriptor_type = MG_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write.image_info = &image_info;

        mg_llapi_renderer_update_descriptor_set(sampler_set, &descriptor_write);
    }

    {
        mg_descriptor_image_info_t image_info;
        image_info.view = frame_texture_view;
        image_info.sampler = sampler;

        mg_descriptor_write_t descriptor_write = { 0 };
        descriptor_write.binding = 0;
        descriptor_write.descriptor_type = MG_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write.image_info = &image_info;

        mg_llapi_renderer_update_descriptor_set(frame_sampler_set, &descriptor_write);
    }

    float x = 0;
    float y = 0;
    float current_x = 0;
    float current_y = 0;

    double current_time, last_time = 0;

    float current_view_size = 27.0f;

    while (is_running)
    {
        current_time = mg_platform_get_time();
        const double delta_time = current_time - last_time;
        last_time = current_time;

            mg_llapi_renderer_begin_frame();

            mg_framebuffer_t framebuffer = mg_llapi_renderer_get_swapchain_framebuffer();

            mg_render_pass_begin_info_t render_pass_begin_info;
            render_pass_begin_info.framebuffer = frame_framebuffer;
            render_pass_begin_info.render_area = (mg_vec4_t){0.0f, 0.0f, viewport.x, viewport.y};
            render_pass_begin_info.clear_value = (mg_vec4_t){0.01f, 0.01f, 0.01f, 1.0f};
            mg_llapi_renderer_begin_render_pass(back_render_pass, &render_pass_begin_info);
            if (mg_input_is_key_down(MG_KEY_Z))
                mg_llapi_renderer_viewport(100, 100);
            else
                mg_llapi_renderer_viewport(viewport.x, viewport.y);

            int32_t width, height;
            mg_platform_get_window_size(platform, &width, &height);

            if (height > 0)
            {
                x+=(mg_input_is_key_down(MG_KEY_D) - mg_input_is_key_down(MG_KEY_A)) * (float)height / (float)width * 3.0f * delta_time * current_view_size;
                y+=(mg_input_is_key_down(MG_KEY_S) - mg_input_is_key_down(MG_KEY_W)) * (float)width / (float)width  * 3.0f * delta_time * current_view_size;
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

            mg_buffer_update_info_t uniform_buffer_update_info;
            uniform_buffer_update_info.size = sizeof(UniformBufferObject);
            uniform_buffer_update_info.data = &ubo;
            mg_llapi_renderer_update_buffer(uniform_buffer, &uniform_buffer_update_info);

            mg_descriptor_buffer_info_t buffer_info;
            buffer_info.buffer = uniform_buffer;
            buffer_info.offset = 0;
            buffer_info.range = sizeof(UniformBufferObject);

            mg_descriptor_write_t descriptor_write = { 0 };
            descriptor_write.binding = 0;
            descriptor_write.descriptor_type = MG_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_write.buffer_info = &buffer_info;

            mg_llapi_renderer_update_descriptor_set(ubo_set, &descriptor_write);

            mg_llapi_renderer_bind_pipeline(pipeline);
            mg_llapi_renderer_bind_vertex_buffer(vertex_buffer);
            mg_llapi_renderer_bind_index_buffer(index_buffer, MG_INDEX_TYPE_UINT32);
            mg_llapi_renderer_bind_descriptor_set(ubo_set, pipeline, 0);
            mg_llapi_renderer_bind_descriptor_set(sampler_set, pipeline, 1);

            PushConstantObject push;
            push.model = mg_mat4_identity();
            push.model = mg_mat4_scale(push.model, (mg_vec3_t){texWidth * 0.01f, texHeight * 0.01f, 1.0f});
            mg_llapi_renderer_push_constants(pipeline, sizeof(PushConstantObject), &push);
            mg_llapi_renderer_draw_indexed(12, 0);

            mg_llapi_renderer_end_render_pass();

            render_pass_begin_info.framebuffer = framebuffer;
            render_pass_begin_info.render_area = (mg_vec4_t){0.0f, 0.0f, viewport.x, viewport.y};
            render_pass_begin_info.clear_value = (mg_vec4_t){0.0f, 0.0f, 0.0f, 1.0f};
            mg_llapi_renderer_begin_render_pass(render_pass, &render_pass_begin_info);
            mg_llapi_renderer_viewport(viewport.x, viewport.y);
            //ubo.projection = mg_mat4_identity();

            mg_llapi_renderer_update_descriptor_set(ubo_set, &descriptor_write);

            mg_llapi_renderer_bind_pipeline(back_pipeline);
            mg_llapi_renderer_bind_vertex_buffer(frame_vertex_buffer);
            mg_llapi_renderer_bind_index_buffer(index_buffer, MG_INDEX_TYPE_UINT32);
            mg_llapi_renderer_bind_descriptor_set(ubo_set, back_pipeline, 0);
            mg_llapi_renderer_bind_descriptor_set(frame_sampler_set, back_pipeline, 1);
            mg_llapi_renderer_draw_indexed(6, 0);

            mg_llapi_renderer_end_render_pass();

            mg_llapi_renderer_end_frame();
            mg_llapi_renderer_present_frame();
        
        mg_platform_poll_messages(platform);
    }

    mg_llapi_renderer_destroy_pipeline(pipeline);
    mg_llapi_renderer_destroy_pipeline(back_pipeline);
    mg_llapi_renderer_destroy_descriptor_set(frame_sampler_set);
    mg_llapi_renderer_destroy_descriptor_set(sampler_set);
    mg_llapi_renderer_destroy_sampler(sampler);
    mg_llapi_renderer_destroy_texture_view(frame_texture_view);
    mg_llapi_renderer_destroy_texture_image(frame_texture_image);
    mg_llapi_renderer_destroy_texture_view(texture_view);
    mg_llapi_renderer_destroy_texture_image(texture_image);
    mg_llapi_renderer_destroy_descriptor_set(ubo_set);
    mg_llapi_renderer_destroy_descriptor_set_layout(sampler_layout);
    mg_llapi_renderer_destroy_descriptor_set_layout(ubo_layout);
    mg_llapi_renderer_destroy_buffer(uniform_buffer);
    mg_llapi_renderer_destroy_buffer(index_buffer);
    mg_llapi_renderer_destroy_buffer(vertex_buffer);
    mg_llapi_renderer_destroy_buffer(frame_vertex_buffer);
    mg_llapi_renderer_destroy_framebuffer(frame_framebuffer);
    mg_llapi_renderer_destroy_render_pass(render_pass);
    mg_llapi_renderer_destroy_render_pass(back_render_pass);

    mg_llapi_renderer_shutdown();
    mg_platform_shutdown(platform);

    return 0;
}