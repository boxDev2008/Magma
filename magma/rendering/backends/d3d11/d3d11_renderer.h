#pragma once

#include "d3d11_types.h"

#if MG_PLATFORM_WINDOWS

extern mg_d3d11_context d3d11_ctx;

void mg_d3d11_renderer_initialize       (mgfx_init_info *init_info);
void mg_d3d11_renderer_shutdown         (void);

void mg_d3d11_renderer_begin      (void);
void mg_d3d11_renderer_end        (void);

void mg_d3d11_renderer_wait             (void);

void mg_d3d11_renderer_configure_swapchain(mg_swapchain_config_info *config_info);

void mg_d3d11_renderer_viewport         (int32_t x, int32_t y, uint32_t width, uint32_t height);
void mg_d3d11_renderer_scissor          (int32_t x, int32_t y, uint32_t width, uint32_t height);

void mg_d3d11_renderer_draw             (uint32_t vertex_count, uint32_t first_vertex);
void mg_d3d11_renderer_draw_indexed     (uint32_t index_count, uint32_t first_index, int32_t first_vertex);

void mg_d3d11_renderer_bind_uniforms    (uint32_t binding, size_t size, void *data);

#endif