#pragma once

#include "opengl_types.inl"
#include "opengl_renderer.h"

#include "../rhi_renderer.h"

mg_opengl_buffer_t *mg_opengl_create_buffer             (mg_buffer_create_info_t *create_info);
void                mg_opengl_destroy_buffer            (mg_opengl_buffer_t *buffer);

void                mg_opengl_update_buffer             (mg_opengl_buffer_t *buffer, mg_buffer_update_info_t *update_info);

void                mg_opengl_bind_vertex_buffer        (mg_opengl_buffer_t *buffer);
void                mg_opengl_bind_index_buffer         (mg_opengl_buffer_t *buffer, mg_index_type_t index_type);