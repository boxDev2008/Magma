#pragma once

#include "base.h"
#include "../rhi_renderer.h"

#include <glad/glad.h>

typedef struct mg_opengl_buffer mg_opengl_buffer_t;
struct mg_opengl_buffer
{
    GLuint id;

    GLenum frequency;
    GLenum usage;
};

typedef struct mg_opengl_pipeline mg_opengl_pipeline_t;
struct mg_opengl_pipeline
{
    GLuint program_id;

    mg_vertex_layout_info_t vertex_layout;
};

typedef struct mg_opengl_context mg_opengl_context_t;
struct mg_opengl_context
{
    GLuint vao;
    GLenum index_type;
};