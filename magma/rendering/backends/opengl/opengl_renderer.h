#pragma once

#include "opengl_types.h"

extern mg_opengl_context gl_ctx;

void mg_opengl_renderer_initialize      (mgfx_init_info *init_info);
void mg_opengl_renderer_shutdown        (void);

void mg_opengl_renderer_begin     (void);
void mg_opengl_renderer_end       (void);

void mg_opengl_renderer_viewport        (int32_t x, int32_t y, uint32_t width, uint32_t height);
void mg_opengl_renderer_scissor         (int32_t x, int32_t y, uint32_t width, uint32_t height);

void mg_opengl_renderer_draw                    (uint32_t vertex_count, uint32_t first_vertex);
void mg_opengl_renderer_draw_indexed            (uint32_t index_count, uint32_t first_index, int32_t first_vertex);

void mg_opengl_renderer_draw_instanced          (uint32_t vertex_count, uint32_t first_vertex, uint32_t instance_count, uint32_t first_instance);
void mg_opengl_renderer_draw_indexed_instanced  (uint32_t index_count, uint32_t first_index, int32_t first_vertex, uint32_t instance_count, uint32_t first_instance);

void mg_opengl_configure_swapchain      (mg_swapchain_config_info *config_info);

void mg_opengl_renderer_bind_uniforms   (uint32_t binding, size_t size, void *data);