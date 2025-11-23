#pragma once

#include "base.h"

#include "../../renderer.h"

#if MG_PLATFORM_EMSCRIPTEN
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

typedef struct mg_opengl_buffer
{
    GLuint id;
    GLenum target;
    GLenum usage;
}
mg_opengl_buffer;

typedef enum mg_opengl_pipeline_type
{
    MG_OPENGL_PIPELINE_TYPE_GRAPHICS,
    MG_OPENGL_PIPELINE_TYPE_COMPUTE
}
mg_opengl_pipeline_type;

typedef struct mg_opengl_pipeline
{
    mg_opengl_pipeline_type type;
    GLuint program_id;
    GLuint sampler_image_location;

    struct
    {
        uint32_t stride;
        uint32_t attribute_count;
        mg_vertex_attribute_info attributes[MG_CONFIG_MAX_VERTEX_ATTRIBUTES];
    }
    vertex_layout;

    GLenum polygon_mode;
    GLenum primitive_topology;
    GLenum cull_mode;
    GLenum front_face;
    mg_color_blend color_blend;

    struct
    {
        bool stencil_test_enabled;
        bool depth_write_enabled;
        GLenum depth_compare_op;
    }
    depth_stencil;
}
mg_opengl_pipeline;

typedef struct mg_opengl_image
{
    GLuint texture_id;
    GLenum texture_target;
    GLenum format;
    GLenum internal_format;
}
mg_opengl_image;

typedef struct mg_opengl_sampler
{
    GLint min_filter;
    GLint mag_filter;

    GLint address_mode_u;
    GLint address_mode_v;
    GLint address_mode_w;
}
mg_opengl_sampler;

typedef struct mg_opengl_render_pass
{
    GLuint framebuffer_id;
}
mg_opengl_render_pass;

typedef struct mg_opengl_context
{
    GLuint vao;
    GLuint uniform_buffers[MG_CONFIG_MAX_BINDABLE_UNIFORMS];

    GLenum index_type;
    //GLenum polygon_mode;
    GLenum primitive_topology;

    struct
    {
        GLuint program;
        GLuint framebuffer;
        GLuint color_attachment;
    }
    back_buffer;

    mg_opengl_pipeline *current_pipeline;
}
mg_opengl_context;