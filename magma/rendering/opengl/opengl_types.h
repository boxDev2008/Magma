#pragma once

#include "base.h"
#include "../rhi_renderer.h"

#include <glad/glad.h>

typedef struct mg_opengl_buffer
{
    GLuint id;
}
mg_opengl_buffer_t,
mg_opengl_vertex_buffer_t, mg_opengl_index_buffer_t;

typedef struct mg_opengl_dynamic_buffer
{
    GLuint id;
}
mg_opengl_dynamic_buffer_t,
mg_opengl_dynamic_vertex_buffer_t, mg_opengl_dynamic_index_buffer_t;

typedef struct mg_opengl_uniform_buffer
{
    GLuint id;
    uint32_t binding;
}
mg_opengl_uniform_buffer_t;

typedef struct mg_opengl_pipeline
{
    GLuint program_id;

    mg_vertex_layout_info_t vertex_layout;
    mg_polygon_mode_t polygon_mode;
    mg_cull_mode_t cull_mode;
    mg_front_face_t front_face;
    mg_color_blend_t color_blend;
    mg_depth_stencil_state_t depth_stencil;

    bool has_push_constants;
    GLuint push_constant_buffer;
}
mg_opengl_pipeline_t;

typedef struct mg_opengl_image
{
    GLuint texture_id;
    GLenum texture_target;
    uint32_t binding;
}
mg_opengl_image_t;

typedef struct mg_opengl_sampler
{
    GLint min_filter;
    GLint mag_filter;

    GLint address_mode_u;
    GLint address_mode_v;
    GLint address_mode_w;
}
mg_opengl_sampler_t;

typedef struct mg_opengl_framebuffer
{
    GLuint id;
}
mg_opengl_framebuffer_t;

typedef struct mg_opengl_render_pass
{
    mg_attachment_info_t color_attachment;
    mg_attachment_info_t depth_stencil_attachment;
    bool has_depth_stencil_attachment;
}
mg_opengl_render_pass_t;

typedef struct mg_opengl_context
{
    GLuint vao;
    GLenum index_type;
    GLenum polygon_mode;

    struct
    {
        GLuint program;
        GLuint framebuffer;
        GLuint color_attachment;
        GLuint vertex_buffer;
        GLuint index_buffer;  
    }
    back_buffer;

    mg_opengl_pipeline_t *current_pipeline;
}
mg_opengl_context_t;