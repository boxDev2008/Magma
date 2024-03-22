#include "opengl_buffer.h"

#include <stdio.h>
#include <stdlib.h>

static GLenum mg_opengl_get_index_type(mg_index_type_t index_type)
{
    switch (index_type)
    {
        case MG_INDEX_TYPE_UINT16: return GL_UNSIGNED_SHORT; break;
        case MG_INDEX_TYPE_UINT32: return GL_UNSIGNED_INT; break;
    }
}

static void mg_opengl_bind_vertex_attributes(void)
{
    const mg_opengl_pipeline_t *pipeline = opengl_context.current_pipeline;
    for (uint32_t i = 0; i < pipeline->vertex_layout.attribute_count; i++)
    {
        switch (pipeline->vertex_layout.attributes[i].format)
        {
        case MG_VERTEX_FORMAT_UINT:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 1,
                GL_UNSIGNED_INT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;
        case MG_VERTEX_FORMAT_INT:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 1,
                GL_INT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;
        case MG_VERTEX_FORMAT_FLOAT:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 1,
                GL_FLOAT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;

        case MG_VERTEX_FORMAT_UINT2:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 2,
                GL_UNSIGNED_INT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;
        case MG_VERTEX_FORMAT_INT2:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 2,
                GL_INT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;
        case MG_VERTEX_FORMAT_FLOAT2:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 2,
                GL_FLOAT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;

        case MG_VERTEX_FORMAT_UINT3:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 3,
                GL_UNSIGNED_INT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;
        case MG_VERTEX_FORMAT_INT3:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 3,
                GL_INT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;
        case MG_VERTEX_FORMAT_FLOAT3:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 3,
                GL_FLOAT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;

        case MG_VERTEX_FORMAT_UINT4:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 4,
                GL_UNSIGNED_INT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;
        case MG_VERTEX_FORMAT_INT4:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 4,
                GL_INT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;
        case MG_VERTEX_FORMAT_FLOAT4:
            glVertexAttribPointer(pipeline->vertex_layout.attributes[i].location, 4,
                GL_FLOAT, GL_FALSE, pipeline->vertex_layout.stride,
                (void*)pipeline->vertex_layout.attributes[i].offset);
            break;
        }

        glEnableVertexAttribArray(pipeline->vertex_layout.attributes[i].location);
    }
}

mg_opengl_vertex_buffer_t *mg_opengl_create_vertex_buffer(size_t size, void *data)
{
    mg_opengl_vertex_buffer_t *buffer = (mg_opengl_vertex_buffer_t*)malloc(sizeof(mg_opengl_vertex_buffer_t));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_vertex_buffer(mg_opengl_vertex_buffer_t *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

mg_opengl_index_buffer_t *mg_opengl_create_index_buffer(size_t size, void *data)
{
    mg_opengl_index_buffer_t *buffer = (mg_opengl_index_buffer_t*)malloc(sizeof(mg_opengl_index_buffer_t));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_index_buffer(mg_opengl_index_buffer_t *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

mg_opengl_dynamic_vertex_buffer_t *mg_opengl_create_dynamic_vertex_buffer(size_t size)
{
    mg_opengl_dynamic_vertex_buffer_t *buffer = (mg_opengl_dynamic_vertex_buffer_t*)malloc(sizeof(mg_opengl_dynamic_vertex_buffer_t));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_dynamic_vertex_buffer(mg_opengl_dynamic_vertex_buffer_t *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

void mg_opengl_update_dynamic_vertex_buffer(mg_opengl_dynamic_vertex_buffer_t *buffer, size_t size, void *data)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

mg_opengl_dynamic_index_buffer_t *mg_opengl_create_dynamic_index_buffer(size_t size)
{
    mg_opengl_dynamic_index_buffer_t *buffer = (mg_opengl_dynamic_index_buffer_t*)malloc(sizeof(mg_opengl_dynamic_index_buffer_t));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_dynamic_index_buffer(mg_opengl_dynamic_index_buffer_t *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

void mg_opengl_update_dynamic_index_buffer(mg_opengl_dynamic_index_buffer_t *buffer, size_t size, void *data)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

mg_opengl_uniform_buffer_t *mg_opengl_create_uniform_buffer(size_t size)
{
    mg_opengl_uniform_buffer_t *buffer = (mg_opengl_uniform_buffer_t*)malloc(sizeof(mg_opengl_uniform_buffer_t));
    glGenBuffers(1, &buffer->id);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer->id);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return buffer;
}

void mg_opengl_destroy_uniform_buffer(mg_opengl_uniform_buffer_t *buffer)
{
    glDeleteBuffers(1, &buffer->id);
    free(buffer);
}

void mg_opengl_update_uniform_buffer(mg_opengl_uniform_buffer_t *buffer, size_t size, uint32_t binding, void *data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, buffer->id);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    buffer->binding = binding;
}

void mg_opengl_bind_vertex_buffer(mg_opengl_vertex_buffer_t *buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    mg_opengl_bind_vertex_attributes();
}

void mg_opengl_bind_dynamic_vertex_buffer(mg_opengl_dynamic_vertex_buffer_t *buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    mg_opengl_bind_vertex_attributes();
}

void mg_opengl_bind_index_buffer(mg_opengl_index_buffer_t *buffer, mg_index_type_t index_type)
{
    opengl_context.index_type = mg_opengl_get_index_type(index_type);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}

void mg_opengl_bind_dynamic_index_buffer(mg_opengl_dynamic_index_buffer_t *buffer, mg_index_type_t index_type)
{
    opengl_context.index_type = mg_opengl_get_index_type(index_type);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id);
}

void mg_opengl_bind_unifom_buffer(mg_opengl_uniform_buffer_t *buffer, mg_opengl_pipeline_t *pipeline)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, buffer->binding, buffer->id);
}