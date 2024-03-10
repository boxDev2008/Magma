#include "opengl_renderer.h"
#include "opengl_pipeline.h"
#include "opengl_buffer.h"
#include "opengl_image.h"
#include "opengl_render_pass.h"

#include "platform/opengl_platform.h"

#if MG_PLATFORM_WINDOWS
#include <windows.h>
#include <wingdi.h>
#endif

#include <stb_image.h>

#define MG_OPENGL_PUSH_CONSTANT_BINDING 4

mg_opengl_context_t opengl_context;

const char *BACK_BUFFER_VERT = "#version 450 core\n"
    "layout (location = 0) in vec3 in_position;\n"
    "layout (location = 1) in vec2 in_tex_coord;\n"
    "out vec2 tex_coord;\n"
    "void main()\n"
    "{\n"
        "gl_Position = vec4(in_position.x, in_position.y, in_position.z, 1.0);\n"
        "tex_coord = in_tex_coord;\n"
    "}\0";

    
const char *BACK_BUFFER_FRAG = "#version 450 core\n"
    "out vec4 out_color;\n"
    "in vec2 tex_coord;\n"
    /*"layout(std140, binding = 1) uniform Ubo {\n"
    "float time;\n"
    "};\n"*/
    "uniform sampler2D u_texture;\n"
    "void main()\n"
    "{\n"
        "out_color = texture(u_texture, tex_coord);"
    "}\0";

void mg_opengl_renderer_initialize(mg_renderer_init_info_t *init_info)
{
    mg_opengl_platform_initialize(init_info->platform);

    glEnable(GL_SCISSOR_TEST);

    glGenVertexArrays(1, &opengl_context.vao);

    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &BACK_BUFFER_VERT, NULL);
    glCompileShader(vertex_shader);
    
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &BACK_BUFFER_FRAG, NULL);
    glCompileShader(fragment_shader);

    opengl_context.back_buffer.program = glCreateProgram();
    glAttachShader(opengl_context.back_buffer.program, vertex_shader);
    glAttachShader(opengl_context.back_buffer.program, fragment_shader);
    glLinkProgram(opengl_context.back_buffer.program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    const float vertices[20] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };

    glGenBuffers(1, &opengl_context.back_buffer.vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, opengl_context.back_buffer.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    const uint16_t indices[8] = {
        0, 1, 3, 1, 2, 3
    };

    glGenBuffers(1, &opengl_context.back_buffer.index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl_context.back_buffer.index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    switch (init_info->swapchain_config_info->format)
    {
        case MG_PIXEL_FORMAT_R8_SRGB:
        case MG_PIXEL_FORMAT_R8G8_SRGB:
        case MG_PIXEL_FORMAT_R8G8B8_SRGB:
        case MG_PIXEL_FORMAT_R8G8B8A8_SRGB:
        glEnable(GL_FRAMEBUFFER_SRGB);
    }

    glGenFramebuffers(1, &opengl_context.back_buffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, opengl_context.back_buffer.framebuffer);

    glGenTextures(1, &opengl_context.back_buffer.color_attachment);
    glBindTexture(GL_TEXTURE_2D, opengl_context.back_buffer.color_attachment);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, init_info->swapchain_config_info->width, init_info->swapchain_config_info->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opengl_context.back_buffer.color_attachment, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void mg_opengl_renderer_shutdown(void)
{
    glDeleteFramebuffers(1, &opengl_context.back_buffer.framebuffer);
    glDeleteTextures(1, &opengl_context.back_buffer.color_attachment);
    glDeleteBuffers(1, &opengl_context.back_buffer.vertex_buffer);
    glDeleteBuffers(1, &opengl_context.back_buffer.index_buffer);
    glDeleteProgram(opengl_context.back_buffer.program);
    glDeleteVertexArrays(1, &opengl_context.vao);
    mg_opengl_platform_shutdown();
}

void mg_opengl_renderer_begin_frame(void)
{
    glBindVertexArray(opengl_context.vao);
    glBindFramebuffer(GL_FRAMEBUFFER, opengl_context.back_buffer.framebuffer);
}

void mg_opengl_renderer_end_frame(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void mg_opengl_renderer_present_frame(void)
{
    glEnable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(opengl_context.back_buffer.program);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, opengl_context.back_buffer.color_attachment);
    glBindBuffer(GL_ARRAY_BUFFER, opengl_context.back_buffer.vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl_context.back_buffer.index_buffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

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
    glDrawArrays(opengl_context.polygon_mode, first_vertex, vertex_count);
}

void mg_opengl_renderer_draw_indexed(uint32_t index_count, uint32_t first_index)
{
    glDrawElements(opengl_context.polygon_mode, index_count, opengl_context.index_type, 0);
}

void mg_opengl_renderer_push_constants(mg_opengl_pipeline_t *pipeline, uint32_t size, void *data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, pipeline->push_constant_buffer);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, MG_OPENGL_PUSH_CONSTANT_BINDING, pipeline->push_constant_buffer);
}

void mg_opengl_configure_swapchain(mg_swapchain_config_info_t *config_info)
{
    glBindTexture(GL_TEXTURE_2D, opengl_context.back_buffer.color_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, config_info->width, config_info->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}