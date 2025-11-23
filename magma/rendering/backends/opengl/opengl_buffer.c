#include "opengl_buffer.h"
#include <stdlib.h>

static GLenum mg_opengl_get_index_type(mg_index_type index_type)
{
    if (index_type == MG_INDEX_TYPE_UINT16)
        return GL_UNSIGNED_SHORT;
    return GL_UNSIGNED_INT;
}

static GLenum mg_opengl_get_buffer_target(mg_buffer_usage usage)
{
    switch (usage)
    {
        case MG_BUFFER_USAGE_VERTEX: return GL_ARRAY_BUFFER;
        case MG_BUFFER_USAGE_INDEX: return GL_ELEMENT_ARRAY_BUFFER;
    }
    return GL_ARRAY_BUFFER;
}

static GLenum mg_opengl_get_data_usage(mg_access_type access)
{
    if (access == MG_ACCESS_TYPE_GPU)
        return GL_STATIC_DRAW;
    return GL_DYNAMIC_DRAW;
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

        glVertexAttribPointer(attr->location, size, type, norm, pipeline->vertex_layout.stride, (const void*)(uintptr_t)attr->offset);
        glEnableVertexAttribArray(attr->location);
    }
}

mg_opengl_buffer *mg_opengl_create_buffer(const mg_buffer_create_info *create_info)
{
    mg_opengl_buffer *buffer = (mg_opengl_buffer*)malloc(sizeof(mg_opengl_buffer));
    buffer->target = mg_opengl_get_buffer_target(create_info->usage);
    buffer->usage = mg_opengl_get_data_usage(create_info->access);
    glGenBuffers(1, &buffer->id);
    glBindBuffer(buffer->target, buffer->id);
    glBufferData(
        buffer->target,
        create_info->size,
        create_info->data,
        buffer->usage
    );
    return buffer;
}

void mg_opengl_update_buffer(mg_opengl_buffer *buffer, size_t size, void *data)
{
    glBindBuffer(buffer->target, buffer->id);
    glBufferData(buffer->target, size, data, buffer->usage);
}

void mg_opengl_bind_vertex_buffer(mg_opengl_buffer *buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    mg_opengl_bind_vertex_attributes();
}

void mg_opengl_bind_index_buffer(mg_opengl_buffer *buffer, mg_index_type index_type)
{
    gl_ctx.index_type = mg_opengl_get_index_type(index_type);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}

void mg_opengl_destroy_buffer(mg_opengl_buffer *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}