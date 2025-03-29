#pragma once

#include "d3d11_renderer.h"

#if MG_PLATFORM_WINDOWS

ID3D11Buffer *mg_d3d11_create_vertex_buffer(size_t size, void *data);
ID3D11Buffer *mg_d3d11_create_index_buffer(size_t size, void *data);

ID3D11Buffer *mg_d3d11_create_dynamic_vertex_buffer(size_t size);
ID3D11Buffer *mg_d3d11_create_dynamic_index_buffer(size_t size);

void mg_d3d11_destroy_buffer(ID3D11Buffer *buffer);

void mg_d3d11_update_dynamic_buffer(ID3D11Buffer *buffer, size_t size, void *data);

void mg_d3d11_bind_vertex_buffer(ID3D11Buffer *buffer);
void mg_d3d11_bind_index_buffer(ID3D11Buffer *buffer, mg_index_type index_type);

#endif