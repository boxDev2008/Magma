#include "opengl_buffer.h"

#include <stdio.h>
#include <stdlib.h>

static GLenum mg_opengl_get_index_type(mg_index_type index_type)
{
    switch (index_type)
    {
        case MG_INDEX_TYPE_UINT16: return GL_UNSIGNED_SHORT; break;
        case MG_INDEX_TYPE_UINT32: return GL_UNSIGNED_INT; break;
    }
}

static void mg_opengl_bind_vertex_attributes(void)
{
    const mg_opengl_pipeline *pipeline = opengl_context.current_pipeline;

    GLenum type;
    GLint size;

    for (uint32_t i = 0; i < pipeline->vertex_layout.attribute_count; i++)
    {
        const mg_vertex_attribute_info *attr = &pipeline->vertex_layout.attributes[i];

        switch (attr->format)
        {
            case MG_VERTEX_FORMAT_UINT:    type = GL_UNSIGNED_INT; size = 1; break;
            case MG_VERTEX_FORMAT_INT:     type = GL_INT;          size = 1; break;
            case MG_VERTEX_FORMAT_FLOAT:   type = GL_FLOAT;        size = 1; break;

            case MG_VERTEX_FORMAT_UINT2:   type = GL_UNSIGNED_INT; size = 2; break;
            case MG_VERTEX_FORMAT_INT2:    type = GL_INT;          size = 2; break;
            case MG_VERTEX_FORMAT_FLOAT2:  type = GL_FLOAT;        size = 2; break;

            case MG_VERTEX_FORMAT_UINT3:   type = GL_UNSIGNED_INT; size = 3; break;
            case MG_VERTEX_FORMAT_INT3:    type = GL_INT;          size = 3; break;
            case MG_VERTEX_FORMAT_FLOAT3:  type = GL_FLOAT;        size = 3; break;

            case MG_VERTEX_FORMAT_UINT4:   type = GL_UNSIGNED_INT; size = 4; break;
            case MG_VERTEX_FORMAT_INT4:    type = GL_INT;          size = 4; break;
            case MG_VERTEX_FORMAT_FLOAT4:  type = GL_FLOAT;        size = 4; break;
        }

        glVertexAttribPointer(attr->location, size, type, GL_FALSE, pipeline->vertex_layout.stride, (void*)attr->offset);
        glEnableVertexAttribArray(attr->location);
    }
}

mg_opengl_vertex_buffer *mg_opengl_create_vertex_buffer(size_t size, void *data)
{
    mg_opengl_vertex_buffer *buffer = (mg_opengl_vertex_buffer*)malloc(sizeof(mg_opengl_vertex_buffer));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_vertex_buffer(mg_opengl_vertex_buffer *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

mg_opengl_index_buffer *mg_opengl_create_index_buffer(size_t size, void *data)
{
    mg_opengl_index_buffer *buffer = (mg_opengl_index_buffer*)malloc(sizeof(mg_opengl_index_buffer));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_index_buffer(mg_opengl_index_buffer *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

mg_opengl_dynamic_vertex_buffer *mg_opengl_create_dynamic_vertex_buffer(size_t size)
{
    mg_opengl_dynamic_vertex_buffer *buffer = (mg_opengl_dynamic_vertex_buffer*)malloc(sizeof(mg_opengl_dynamic_vertex_buffer));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_dynamic_vertex_buffer(mg_opengl_dynamic_vertex_buffer *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

void mg_opengl_update_dynamic_vertex_buffer(mg_opengl_dynamic_vertex_buffer *buffer, size_t size, void *data)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

mg_opengl_dynamic_index_buffer *mg_opengl_create_dynamic_index_buffer(size_t size)
{
    mg_opengl_dynamic_index_buffer *buffer = (mg_opengl_dynamic_index_buffer*)malloc(sizeof(mg_opengl_dynamic_index_buffer));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_dynamic_index_buffer(mg_opengl_dynamic_index_buffer *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

void mg_opengl_update_dynamic_index_buffer(mg_opengl_dynamic_index_buffer *buffer, size_t size, void *data)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

mg_opengl_uniform_buffer *mg_opengl_create_uniform_buffer(size_t size)
{
    mg_opengl_uniform_buffer *buffer = (mg_opengl_uniform_buffer*)malloc(sizeof(mg_opengl_uniform_buffer));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer->id);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_uniform_buffer(mg_opengl_uniform_buffer *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

void mg_opengl_update_uniform_buffer(mg_opengl_uniform_buffer *buffer, size_t size, void *data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, buffer->id);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
}

void mg_opengl_bind_vertex_buffer(mg_opengl_vertex_buffer *buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    mg_opengl_bind_vertex_attributes();
}

void mg_opengl_bind_dynamic_vertex_buffer(mg_opengl_dynamic_vertex_buffer *buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    mg_opengl_bind_vertex_attributes();
}

void mg_opengl_bind_index_buffer(mg_opengl_index_buffer *buffer, mg_index_type index_type)
{
    opengl_context.index_type = mg_opengl_get_index_type(index_type);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}

void mg_opengl_bind_dynamic_index_buffer(mg_opengl_dynamic_index_buffer *buffer, mg_index_type index_type)
{
    opengl_context.index_type = mg_opengl_get_index_type(index_type);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}

void mg_opengl_bind_unifom_buffer(mg_opengl_uniform_buffer *buffer)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffer->id);
}