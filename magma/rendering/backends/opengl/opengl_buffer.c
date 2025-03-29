#include "opengl_buffer.h"

#include <stdio.h>
#include <stdlib.h>

static GLenum mg_opengl_get_index_type(mg_index_type index_type)
{
    if (index_type == MG_INDEX_TYPE_UINT16)
        return GL_UNSIGNED_SHORT;
    return GL_UNSIGNED_INT;
}

static void mg_opengl_bind_vertex_attributes(void)
{
    const mg_opengl_pipeline *pipeline = gl_ctx.current_pipeline;

    GLenum type;
    GLint size;
    GLboolean norm;

    for (uint32_t i = 0; i < pipeline->vertex_layout.attribute_count; i++)
    {
        const mg_vertex_attribute_info *attr = &pipeline->vertex_layout.attributes[i];

        switch (attr->format)
        {
            case MG_VERTEX_FORMAT_UINT:    type = GL_UNSIGNED_INT; size = 1; norm = GL_FALSE; break;
            case MG_VERTEX_FORMAT_INT:     type = GL_INT;          size = 1; norm = GL_FALSE; break;
            case MG_VERTEX_FORMAT_FLOAT:   type = GL_FLOAT;        size = 1; norm = GL_FALSE; break;

            case MG_VERTEX_FORMAT_UINT2:   type = GL_UNSIGNED_INT; size = 2; norm = GL_FALSE; break;
            case MG_VERTEX_FORMAT_INT2:    type = GL_INT;          size = 2; norm = GL_FALSE; break;
            case MG_VERTEX_FORMAT_FLOAT2:  type = GL_FLOAT;        size = 2; norm = GL_FALSE; break;

            case MG_VERTEX_FORMAT_UINT3:   type = GL_UNSIGNED_INT; size = 3; norm = GL_FALSE; break;
            case MG_VERTEX_FORMAT_INT3:    type = GL_INT;          size = 3; norm = GL_FALSE; break;
            case MG_VERTEX_FORMAT_FLOAT3:  type = GL_FLOAT;        size = 3; norm = GL_FALSE; break;

            case MG_VERTEX_FORMAT_UINT4:   type = GL_UNSIGNED_INT; size = 4; norm = GL_FALSE; break;
            case MG_VERTEX_FORMAT_INT4:    type = GL_INT;          size = 4; norm = GL_FALSE; break;
            case MG_VERTEX_FORMAT_FLOAT4:  type = GL_FLOAT;        size = 4; norm = GL_FALSE; break;

            case MG_VERTEX_FORMAT_UBYTE4:  type = GL_UNSIGNED_BYTE; size = 4; norm = GL_TRUE; break;
            case MG_VERTEX_FORMAT_BYTE4:   type = GL_BYTE;          size = 4; norm = GL_TRUE; break;

            case MG_VERTEX_FORMAT_UBYTE4N: type = GL_UNSIGNED_BYTE; size = 4; norm = GL_TRUE; break;
            case MG_VERTEX_FORMAT_BYTE4N:  type = GL_BYTE;          size = 4; norm = GL_TRUE; break;
        }

        glVertexAttribPointer(attr->location, size, type, norm, pipeline->vertex_layout.stride, (void*)attr->offset);
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
    gl_ctx.index_type = mg_opengl_get_index_type(index_type);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}

void mg_opengl_bind_dynamic_index_buffer(mg_opengl_dynamic_index_buffer *buffer, mg_index_type index_type)
{
    gl_ctx.index_type = mg_opengl_get_index_type(index_type);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}