#pragma once

#include "d3d11_renderer.h"

#if MG_PLATFORM_WINDOWS

mg_d3d11_buffer *mg_d3d11_create_buffer(const mg_buffer_create_info *create_info);
void mg_d3d11_update_buffer(mg_d3d11_buffer *buffer, size_t size, void *data);
void mg_d3d11_bind_vertex_buffer(mg_d3d11_buffer *buffer);
void mg_d3d11_bind_index_buffer(mg_d3d11_buffer *buffer, mg_index_type index_type);
void mg_d3d11_destroy_buffer(mg_d3d11_buffer *buffer);

#endif