#pragma once

#include "opengl_renderer.h"

#include "../../lowl_renderer.h"

mg_opengl_vertex_buffer		   *mg_opengl_create_vertex_buffer			(size_t size, void *data);
void                                mg_opengl_destroy_vertex_buffer			(mg_opengl_vertex_buffer *buffer);

mg_opengl_index_buffer		   *mg_opengl_create_index_buffer			(size_t size, void *data);
void                                mg_opengl_destroy_index_buffer			(mg_opengl_index_buffer *buffer);

mg_opengl_dynamic_vertex_buffer  *mg_opengl_create_dynamic_vertex_buffer	(size_t size);
void                                mg_opengl_destroy_dynamic_vertex_buffer	(mg_opengl_dynamic_vertex_buffer *buffer);
void                                mg_opengl_update_dynamic_vertex_buffer	(mg_opengl_dynamic_vertex_buffer *buffer, size_t size, void *data);

mg_opengl_dynamic_index_buffer   *mg_opengl_create_dynamic_index_buffer	(size_t size);
void                                mg_opengl_destroy_dynamic_index_buffer	(mg_opengl_dynamic_index_buffer *buffer);
void                                mg_opengl_update_dynamic_index_buffer	(mg_opengl_dynamic_index_buffer *buffer, size_t size, void *data);

mg_opengl_uniform_buffer		   *mg_opengl_create_uniform_buffer			(size_t size);
void                                mg_opengl_destroy_uniform_buffer		(mg_opengl_uniform_buffer *buffer);
void                                mg_opengl_update_uniform_buffer			(mg_opengl_uniform_buffer *buffer, size_t size, void *data);

void                                mg_opengl_bind_vertex_buffer			(mg_opengl_vertex_buffer *buffer);
void                                mg_opengl_bind_dynamic_vertex_buffer	(mg_opengl_dynamic_vertex_buffer *buffer);

void                                mg_opengl_bind_index_buffer				(mg_opengl_index_buffer *buffer, mg_index_type index_type);
void                                mg_opengl_bind_dynamic_index_buffer		(mg_opengl_dynamic_index_buffer *buffer, mg_index_type index_type);

void                                mg_opengl_bind_unifom_buffer			(mg_opengl_uniform_buffer *buffer);