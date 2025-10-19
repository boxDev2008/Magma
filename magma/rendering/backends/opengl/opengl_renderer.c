#include "opengl_renderer.h"
#include "opengl_pipeline.h"
#include "opengl_buffer.h"
#include "opengl_image.h"
#include "opengl_render_pass.h"

#include "platform/opengl_platform.h"

#if MG_PLATFORM_WINDOWS

#include <windows.h>
#include <wingdi.h>

#pragma comment (lib, "gdi32")
#pragma comment (lib, "opengl32")

#endif

mg_opengl_context gl_ctx;

const char *BACK_BUFFER_VERT =
#if MG_PLATFORM_EMSCRIPTEN
    "#version 300 es\n"
    "precision mediump float;"
#else
    "#version 450 core\n"
#endif
    "const vec2 positions[4] = vec2[]("
        "vec2(-1.0, -1.0),"
        "vec2(1.0, -1.0),"
        "vec2(-1.0, 1.0),"
        "vec2(1.0, 1.0)"
    ");"
    "const vec2 tex_coords[4] = vec2[]("
        "vec2(0.0, 0.0),"
        "vec2(1.0, 0.0),"
        "vec2(0.0, 1.0),"
        "vec2(1.0, 1.0)"
    ");"
    "out vec2 tex_coord;"
    "void main()"
    "{"
        "gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);"
        "gl_Position.y = -gl_Position.y;"
        "tex_coord = tex_coords[gl_VertexID];"
    "}\0";

const char *BACK_BUFFER_FRAG =
#if MG_PLATFORM_EMSCRIPTEN
    "#version 300 es\n"
    "precision mediump float;"
#else
    "#version 450 core\n"
#endif
    "out vec4 out_color;\n"
    "in vec2 tex_coord;"
    "uniform sampler2D u_texture;"
    "void main()"
    "{"
        "out_color = texture(u_texture, tex_coord);"
    "}\0";

void mg_opengl_renderer_initialize(mgfx_init_info *init_info)
{
    mg_opengl_platform_initialize();

    glGenVertexArrays(1, &gl_ctx.vao);

    uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &BACK_BUFFER_VERT, NULL);
    glCompileShader(vertex);
    
    uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &BACK_BUFFER_FRAG, NULL);
    glCompileShader(fragment);

    gl_ctx.back_buffer.program = glCreateProgram();
    glAttachShader(gl_ctx.back_buffer.program, vertex);
    glAttachShader(gl_ctx.back_buffer.program, fragment);
    glLinkProgram(gl_ctx.back_buffer.program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    glGenFramebuffers(1, &gl_ctx.back_buffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gl_ctx.back_buffer.framebuffer);

    glGenTextures(1, &gl_ctx.back_buffer.color_attachment);
    glBindTexture(GL_TEXTURE_2D, gl_ctx.back_buffer.color_attachment);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    mg_opengl_configure_swapchain(init_info->swapchain_config_info);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_ctx.back_buffer.color_attachment, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenBuffers(MG_CONFIG_MAX_BINDABLE_UNIFORMS, gl_ctx.uniform_buffers);

    for (uint32_t i = 0; i < MG_CONFIG_MAX_BINDABLE_IMAGES; i++)
        gl_ctx.sampler_indices[i] = i;
}

void mg_opengl_renderer_shutdown(void)
{
    glDeleteBuffers(MG_CONFIG_MAX_BINDABLE_UNIFORMS, gl_ctx.uniform_buffers);
    glDeleteFramebuffers(1, &gl_ctx.back_buffer.framebuffer);
    glDeleteTextures(1, &gl_ctx.back_buffer.color_attachment);
    glDeleteProgram(gl_ctx.back_buffer.program);
    glDeleteVertexArrays(1, &gl_ctx.vao);
    mg_opengl_platform_shutdown();
}

void mg_opengl_renderer_begin(void)
{
	glEnable(GL_SCISSOR_TEST);
    glBindVertexArray(gl_ctx.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, gl_ctx.back_buffer.framebuffer);
}

void mg_opengl_renderer_end(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_SCISSOR_TEST);

    glEnable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(gl_ctx.back_buffer.program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_ctx.back_buffer.color_attachment);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mg_opengl_platform_swapbuffers();
}

void mg_opengl_renderer_wait(void)
{

}

void mg_opengl_renderer_viewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

void mg_opengl_renderer_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    glScissor(x, y, width, height);
}

void mg_opengl_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    glDrawArrays(gl_ctx.primitive_topology, first_vertex, vertex_count);
}

void mg_opengl_renderer_draw_indexed(uint32_t index_count, uint32_t first_index, int32_t first_vertex)
{
	const int index_size = (gl_ctx.index_type == GL_UNSIGNED_SHORT) ? 2 : 4;
#if !MG_PLATFORM_EMSCRIPTEN
	glDrawElementsBaseVertex(gl_ctx.primitive_topology, index_count, gl_ctx.index_type, (void*)(uintptr_t)(first_index * index_size), first_vertex);
#else
    glDrawElements(gl_ctx.primitive_topology, index_count, gl_ctx.index_type, (void*)(uintptr_t)(first_index * index_size));
#endif
}

void mg_opengl_configure_swapchain(mg_swapchain_config_info *config_info)
{
    glBindTexture(GL_TEXTURE_2D, gl_ctx.back_buffer.color_attachment);
    switch (config_info->format)
    {
    case MG_PIXEL_FORMAT_R8_SRGB:
    case MG_PIXEL_FORMAT_R8G8_SRGB:
    case MG_PIXEL_FORMAT_R8G8B8_SRGB:
    case MG_PIXEL_FORMAT_R8G8B8A8_SRGB:
    case MG_PIXEL_FORMAT_B8G8R8_SRGB:
    case MG_PIXEL_FORMAT_B8G8R8A8_SRGB:
#if !MG_PLATFORM_EMSCRIPTEN
        glEnable(GL_FRAMEBUFFER_SRGB);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, config_info->width, config_info->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        break;
    default:
#if !MG_PLATFORM_EMSCRIPTEN
        glDisable(GL_FRAMEBUFFER_SRGB);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, config_info->width, config_info->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    switch (config_info->present_mode)
    {
    case MG_PRESENT_MODE_IMMEDIATE:
    case MG_PRESENT_MODE_MAILBOX:
        mg_opengl_platform_set_vsync(false);
        break;
    default:
        mg_opengl_platform_set_vsync(true);
        break;
    }
}

void mg_opengl_renderer_bind_uniforms(uint32_t binding, size_t size, void *data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, gl_ctx.uniform_buffers[binding]);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, gl_ctx.uniform_buffers[binding]);
}