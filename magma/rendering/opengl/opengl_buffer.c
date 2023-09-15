#include "opengl_buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

GLuint mg_opengl_get_buffer_usage(mg_buffer_usage_t usage)
{
    switch (usage)
    {
        case MG_BUFFER_USAGE_VERTEX: return GL_ARRAY_BUFFER; break;
        case MG_BUFFER_USAGE_INDEX: return GL_ELEMENT_ARRAY_BUFFER; break;
        case MG_BUFFER_USAGE_UNIFORM: return GL_UNIFORM_BUFFER; break;
        case MG_BUFFER_USAGE_STORAGE: return GL_SHADER_STORAGE_BUFFER; break;
    }
}

GLenum mg_opengl_get_buffer_update_frequency(mg_buffer_update_frequency_t frequency)
{
    switch (frequency)
    {
        case MG_BUFFER_UPDATE_FREQUENCY_STATIC: return GL_STATIC_DRAW; break;
        case MG_BUFFER_UPDATE_FREQUENCY_DYNAMIC: return GL_DYNAMIC_DRAW; break;
    }
}

GLenum mg_opengl_get_index_type(mg_index_type_t index_type)
{
    switch (index_type)
    {
        case MG_INDEX_TYPE_UINT16: return GL_UNSIGNED_SHORT; break;
        case MG_INDEX_TYPE_UINT32: return GL_UNSIGNED_INT; break;
    }
}

mg_opengl_buffer_t *mg_opengl_create_buffer(mg_buffer_create_info_t *create_info)
{
    mg_opengl_buffer_t *buffer = (mg_opengl_buffer_t*)malloc(sizeof(mg_opengl_buffer_t));
    buffer->frequency = mg_opengl_get_buffer_update_frequency(create_info->frequency);
    buffer->usage = mg_opengl_get_buffer_usage(create_info->usage);

    glGenBuffers(1, &buffer->id);
    glBindBuffer(buffer->usage, buffer->id);
    glBufferData(buffer->usage, create_info->size, NULL, buffer->frequency);
    //glBindBuffer(buffer->usage, 0);

    return buffer;
}

void mg_opengl_update_buffer(mg_opengl_buffer_t *buffer, mg_buffer_update_info_t *update_info)
{
    glBindBuffer(buffer->usage, buffer->id);
    glBufferData(buffer->usage, update_info->size, update_info->data, buffer->frequency);
    //glBindBuffer(buffer->usage, 0);
}

void mg_opengl_destroy_buffer(mg_opengl_buffer_t *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

void mg_opengl_bind_vertex_buffer(mg_opengl_buffer_t *buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
}

void mg_opengl_bind_index_buffer(mg_opengl_buffer_t *buffer, mg_index_type_t index_type)
{
    opengl_context.index_type = mg_opengl_get_index_type(index_type);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}