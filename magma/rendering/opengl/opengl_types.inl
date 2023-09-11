#pragma once

#include "base.h"
#include "../llapi_renderer.h"

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
    GLuint vao_id;
    GLuint program_id;
};

typedef struct mg_opengl_context mg_opengl_context_t;
struct mg_opengl_context
{
    GLenum index_type;
};