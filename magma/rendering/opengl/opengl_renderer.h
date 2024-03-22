#pragma once

#include "opengl_types.h"

extern mg_opengl_context_t opengl_context;

void mg_opengl_renderer_initialize      (mg_renderer_init_info_t *init_info);
void mg_opengl_renderer_shutdown        (void);

void mg_opengl_renderer_begin_frame     (void);
void mg_opengl_renderer_end_frame       (void);

void mg_opengl_renderer_present_frame	(void);

void mg_opengl_renderer_wait            (void);

void mg_opengl_renderer_viewport        (int32_t x, int32_t y, uint32_t width, uint32_t height);
void mg_opengl_renderer_scissor         (int32_t x, int32_t y, uint32_t width, uint32_t height);

void mg_opengl_renderer_draw            (uint32_t vertex_count, uint32_t first_vertex);
void mg_opengl_renderer_draw_indexed    (uint32_t index_count, uint32_t first_index);

void mg_opengl_renderer_push_constants  (mg_opengl_pipeline_t *pipeline, uint32_t size, void *data);

void mg_opengl_configure_swapchain      (mg_swapchain_config_info_t *config_info);