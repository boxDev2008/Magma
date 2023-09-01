#include "magma/core/event.h"
#include "magma/core/event_types.h"

#include "magma/platform/platform.h"

#include "magma/rendering/llapi_renderer.h"

#include "magma/rendering/vulkan/shaders/hello_triangle/vert.spv.h"
#include "magma/rendering/vulkan/shaders/hello_triangle/frag.spv.h"

static bool is_running = true;

void on_application_quit(mg_application_quit_event_data_t *data)
{
    is_running = false;
}

void on_resize(mg_resized_event_data_t *data)
{
    mg_llapi_renderer_resize(data->width, data->height);
}

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
        0.0f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f
    };

    mg_buffer_create_info_t vertex_buffer_create_info;
    vertex_buffer_create_info.usage = MG_BUFFER_USAGE_VERTEX;
    vertex_buffer_create_info.size = sizeof(vertices);
    vertex_buffer_create_info.mapped_at_creation = true;
    mg_buffer_t vertex_buffer =
        mg_llapi_renderer_create_buffer(&vertex_buffer_create_info);
    
    mg_buffer_update_info_t vertex_buffer_update_info;
    vertex_buffer_update_info.size = sizeof(vertices);
    vertex_buffer_update_info.data = vertices;
    mg_llapi_renderer_update_buffer(vertex_buffer, &vertex_buffer_update_info);

    mg_shader_source_t vertex_shader;
    vertex_shader.code = VULKAN_HELLO_TRIANGLE_VERT;
    vertex_shader.code_size = sizeof(VULKAN_HELLO_TRIANGLE_VERT);

    mg_shader_source_t fragment_shader;
    fragment_shader.code = VULKAN_HELLO_TRIANGLE_FRAG;
    fragment_shader.code_size = sizeof(VULKAN_HELLO_TRIANGLE_FRAG);

    mg_vertex_attribute_info_t vertex_attributes[1];
    vertex_attributes[0].location = 0;
    vertex_attributes[0].offset = 0;
    vertex_attributes[0].format = MG_VERTEX_FORMAT_FLOAT2;

    mg_vertex_layout_info_t vertex_layout;
    vertex_layout.stride = 2 * sizeof(float);
    vertex_layout.attributes = vertex_attributes;
    vertex_layout.attribute_count = 1;

    mg_program_create_info_t program_create_info;
    program_create_info.vertex_shader = vertex_shader;
    program_create_info.fragment_shader = fragment_shader;
    program_create_info.vertex_layout = vertex_layout;
    program_create_info.polygon_mode = MG_POLYGON_MODE_FILL;
    program_create_info.cull_mode = MG_CULL_MODE_BACK;
    program_create_info.front_face = MG_FRONT_FACE_CW;
    program_create_info.has_push_constants = false;

    mg_program_t program = mg_llapi_renderer_create_program(&program_create_info);
    
    while (is_running)
    {
        mg_llapi_renderer_begin_frame();

        mg_llapi_renderer_bind_program(program);
        mg_llapi_renderer_bind_vertex_buffer(vertex_buffer);
        mg_llapi_renderer_draw(3, 0);

        mg_llapi_renderer_end_frame();
        mg_llapi_renderer_present_frame();
        
        mg_platform_poll_messages(&platform);
    }

    mg_llapi_renderer_destroy_program(program);
    mg_llapi_renderer_destroy_buffer(vertex_buffer);

    mg_llapi_renderer_shutdown();
    mg_platform_shutdown(&platform);

    return 0;
}