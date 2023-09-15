#include "opengl_renderer.h"
#include "opengl_pipeline.h"
#include "opengl_buffer.h"

#include "platform/opengl_platform.h"

#if MG_PLATFORM_WINDOWS
#include <windows.h>
#include <wingdi.h>
#endif

mg_opengl_context_t opengl_context;
static mg_opengl_pipeline_t *pipeline;
static mg_opengl_buffer_t *vertex_buffer;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

unsigned int VAO, VBO;
unsigned int shaderpipeline;

void mg_opengl_renderer_initialize(mg_renderer_init_info_t *init_info)
{
    mg_opengl_platform_initialize(init_info->platform);

    glGenVertexArrays(1, &opengl_context.vao);

    mg_shader_source_t vertex_shader = {
        .code = vertexShaderSource,
        .code_size = sizeof(vertexShaderSource)
    };

    mg_shader_source_t fragment_shader = {
        .code  = fragmentShaderSource,
        .code_size = sizeof(fragmentShaderSource)
    };

    mg_pipeline_create_info_t pipeline_create_info = {
        .vertex_shader = &vertex_shader,
        .fragment_shader = &fragment_shader
    };
    pipeline = mg_opengl_create_pipeline(&pipeline_create_info);

    const float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  

    mg_buffer_create_info_t vertex_buffer_create_info = {
        .usage = MG_BUFFER_USAGE_VERTEX,
        .frequency = MG_BUFFER_UPDATE_FREQUENCY_STATIC,
        .size = sizeof(vertices)
    };

    vertex_buffer = mg_opengl_create_buffer(&vertex_buffer_create_info);

    mg_buffer_update_info_t buffer_update_info = {
        .data = vertices,
        .size = sizeof(vertices)
    };

    mg_opengl_update_buffer(vertex_buffer, &buffer_update_info);
}

void mg_opengl_renderer_shutdown(void)
{
    mg_opengl_destroy_buffer(vertex_buffer);
    mg_opengl_destroy_pipeline(pipeline);
    mg_opengl_platform_shutdown();
}

void mg_opengl_renderer_begin_frame(void)
{

}

void mg_opengl_renderer_end_frame(void)
{

}

void mg_opengl_renderer_present(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(opengl_context.vao);

    mg_opengl_bind_pipeline(pipeline);
    mg_opengl_bind_vertex_buffer(vertex_buffer);
    mg_opengl_renderer_draw(3, 0);

    mg_opengl_platform_swapbuffers();
}

void mg_opengl_renderer_wait(void)
{

}

void mg_opengl_renderer_viewport(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
}

void mg_opengl_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    glDrawArrays(GL_TRIANGLES, first_vertex, vertex_count);
}

void mg_opengl_renderer_draw_indexed(uint32_t index_count, uint32_t first_index)
{
    glDrawElements(GL_TRIANGLES, index_count, opengl_context.index_type, 0);
}

void mg_opengl_renderer_push_constants(mg_opengl_pipeline_t *pipeline, uint32_t size, void *data)
{

}