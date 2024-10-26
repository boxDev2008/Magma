#pragma once

#include "base.h"

#include "../../lowl_renderer.h"
#include <glad/glad.h>

typedef struct mg_opengl_buffer
{
    GLuint id;
}
mg_opengl_buffer,
mg_opengl_vertex_buffer, mg_opengl_index_buffer;

typedef struct mg_opengl_dynamic_buffer
{
    GLuint id;
}
mg_opengl_dynamic_buffer,
mg_opengl_dynamic_vertex_buffer, mg_opengl_dynamic_index_buffer,
mg_opengl_uniform_buffer;

typedef struct mg_opengl_pipeline
{
    GLuint program_id;

    struct
    {
        uint32_t stride;
        uint32_t attribute_count;
        mg_vertex_attribute_info attributes[MG_CONFIG_MAX_VERTEX_ATTRIBUTES];
    }
    vertex_layout;
    
    //mg_vertex_layout_info vertex_layout;
    GLenum polygon_mode;
    GLenum primitive_topology;
    GLenum cull_mode;
    GLenum front_face;
    mg_color_blend color_blend;

    struct
    {
        bool depth_test_enable;
        bool stencil_test_enable;
        GLenum depth_compare_op;
    }
    depth_stencil;

    bool has_push_constants;
    GLuint push_constant_buffer;
}
mg_opengl_pipeline;

typedef struct mg_opengl_image
{
    GLuint texture_id;
    GLenum texture_target;
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

typedef struct mg_opengl_image_array
{
    mg_opengl_image **images;
    mg_opengl_sampler **samplers;
    uint32_t count;
}
mg_opengl_image_array;

typedef struct mg_opengl_framebuffer
{
    GLuint id;
}
mg_opengl_framebuffer;

typedef struct mg_opengl_render_pass mg_opengl_render_pass;

typedef struct mg_opengl_context
{
    GLuint vao;
    GLenum index_type;
    //GLenum polygon_mode;
    GLenum primitive_topology;

    struct
    {
        GLuint program;
        GLuint framebuffer;
        GLuint color_attachment;
        GLuint vertex_buffer;
        GLuint index_buffer;  
    }
    back_buffer;

    mg_opengl_pipeline *current_pipeline;
}
mg_opengl_context;