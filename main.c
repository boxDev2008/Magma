#include "magma/core/event.h"
#include "magma/core/event_types.h"

#include "magma/platform/platform.h"

#include "magma/rendering/llapi_renderer.h"

#include "magma/math/mat4.h"

#include "magma/rendering/vulkan/shaders/hello_triangle/vert.spv.h"
#include "magma/rendering/vulkan/shaders/hello_triangle/frag.spv.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static bool is_running = true;

void on_application_quit(mg_application_quit_event_data_t *data)
{
    is_running = false;
}

void on_resize(mg_resized_event_data_t *data)
{
    mg_llapi_renderer_resize(data->width, data->height);
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
    mg_platform_t platform;
    mg_window_create_info_t win_description = {
        .title = "Hello uwu!",
        .width = 800,
        .height = 600,
        .position_x = 200,
        .position_y = 100
    };

    mg_event_register(MG_EVENT_CODE_APPLICATION_QUIT, (PFN_on_event)on_application_quit);
    mg_platform_initialize(&platform, &win_description);
    mg_event_register(MG_EVENT_CODE_RESIZED, (PFN_on_event)on_resize);
    mg_llapi_renderer_initialize(&platform, MG_RENDERER_TYPE_VULKAN);

    const float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 1.0f
    };

    const uint32_t indices[] = {
        0, 1, 3, 1, 2, 3
    };

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

    mg_program_create_info_t program_create_info;

    mg_descriptor_set_layout_t layouts[] = {
        ubo_layout, sampler_layout
    };

    program_create_info.desctriptor_set_layout_count = 2;
    program_create_info.desctriptor_set_layouts = layouts;

    program_create_info.vertex_shader = &vertex_shader;
    program_create_info.fragment_shader = &fragment_shader;

    program_create_info.vertex_layout = vertex_layout;

    program_create_info.polygon_mode = MG_POLYGON_MODE_FILL;
    program_create_info.cull_mode = MG_CULL_MODE_BACK;
    program_create_info.front_face = MG_FRONT_FACE_CW;
    
    program_create_info.has_push_constants = true;
    program_create_info.push_constants_size = sizeof(PushConstantObject);

    mg_program_t program = mg_llapi_renderer_create_program(&program_create_info);

    mg_texture_image_create_info_t texture_image_create_info = { 0 };
    int texChannels;
    texture_image_create_info.data = stbi_load("texture.png", &texture_image_create_info.width, &texture_image_create_info.height, &texChannels, STBI_rgb_alpha);
    
    mg_texture_image_t texture_image =
        mg_llapi_renderer_create_texture_image(&texture_image_create_info);

    stbi_image_free(texture_image_create_info.data);

    mg_sampler_create_info_t sampler_create_info = { 0 };
    sampler_create_info.mag_filter = MG_SAMPLER_FILTER_LINEAR;
    sampler_create_info.min_filter = MG_SAMPLER_FILTER_LINEAR;
    sampler_create_info.address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info.address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_create_info.address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    mg_sampler_t sampler =
        mg_llapi_renderer_create_sampler(&sampler_create_info);

    mg_descriptor_set_t sampler_set;
    mg_descriptor_set_create_info_t sampler_set_create_info;
    sampler_set_create_info.layouts = &sampler_layout;
    sampler_set = mg_llapi_renderer_create_descriptor_set(&sampler_set_create_info);

    {
        mg_descriptor_image_info_t image_info;
        image_info.image = texture_image;
        image_info.sampler = sampler;

        mg_descriptor_write_t descriptor_write = { 0 };
        descriptor_write.binding = 0;
        descriptor_write.descriptor_type = MG_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write.image_info = &image_info;

        mg_llapi_renderer_update_descriptor_set(sampler_set, &descriptor_write);
    }

    while (is_running)
    {
        mg_llapi_renderer_begin_frame();

        int32_t width, height;
        mg_platform_get_window_size(&platform, &width, &height);
        
        ubo.projection = mg_mat4_ortho(height * 0.5f, -height * 0.5f, -width * 0.5f, width * 0.5f, -1.0f, 1.0f);
        
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

        mg_llapi_renderer_bind_program(program);
        mg_llapi_renderer_bind_vertex_buffer(vertex_buffer);
        mg_llapi_renderer_bind_index_buffer(index_buffer, MG_INDEX_TYPE_UINT32);
        mg_llapi_bind_descriptor_set(ubo_set, program, 0);
        mg_llapi_bind_descriptor_set(sampler_set, program, 1);
        PushConstantObject pushConstantObject;
        pushConstantObject.model = mg_mat4_identity();
        pushConstantObject.model = mg_mat4_scale(pushConstantObject.model, (mg_vec3_t){457.6f, 123.2f, 1.0f});
        mg_llapi_renderer_push_constants(program, sizeof(PushConstantObject), &pushConstantObject);
        mg_llapi_renderer_draw_indexed(6, 0);

        mg_llapi_renderer_end_frame();
        mg_llapi_renderer_present_frame();
        
        mg_platform_poll_messages(&platform);
    }

    mg_llapi_renderer_destroy_program(program);
    mg_llapi_renderer_destroy_descriptor_set(sampler_set);
    mg_llapi_renderer_destroy_sampler(sampler);
    mg_llapi_renderer_destroy_texture_image(texture_image);
    mg_llapi_renderer_destroy_descriptor_set(ubo_set);
    mg_llapi_renderer_destroy_descriptor_set_layout(sampler_layout);
    mg_llapi_renderer_destroy_descriptor_set_layout(ubo_layout);
    mg_llapi_renderer_destroy_buffer(uniform_buffer);
    mg_llapi_renderer_destroy_buffer(index_buffer);
    mg_llapi_renderer_destroy_buffer(vertex_buffer);

    mg_llapi_renderer_shutdown();
    mg_platform_shutdown(&platform);

    return 0;
}