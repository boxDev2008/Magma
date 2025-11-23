#pragma once

#include "opengl_renderer.h"

mg_opengl_buffer *mg_opengl_create_buffer(const mg_buffer_create_info *create_info);
void mg_opengl_update_buffer(mg_opengl_buffer *buffer, size_t size, void *data);
void mg_opengl_destroy_buffer(mg_opengl_buffer *buffer);

void mg_opengl_bind_vertex_buffer(mg_opengl_buffer *buffer);
void mg_opengl_bind_index_buffer(mg_opengl_buffer *buffer, mg_index_type index_type);