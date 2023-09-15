#pragma once

#include "vulkan_types.inl"

#if MG_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
    #define MG_VULKAN_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif BX_PLATFORM_LINUX
    #define VK_USE_PLATFORM_XLIB_KHR
    #define VK_USE_PLATFORM_XCB_KHR
    #define MG_VULKAN_SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif

extern mg_vulkan_context_t vulkan_context;

void mg_vulkan_renderer_initialize      (mg_renderer_init_info_t *init_info);
void mg_vulkan_renderer_shutdown        (void);

void mg_vulkan_renderer_begin_frame     (void);
void mg_vulkan_renderer_end_frame       (void);

void mg_vulkan_renderer_present         (void);

void mg_vulkan_renderer_wait            (void);

void mg_vulkan_renderer_viewport        (uint32_t width, uint32_t height);

void mg_vulkan_renderer_draw            (uint32_t vertex_count, uint32_t first_vertex);
void mg_vulkan_renderer_draw_indexed    (uint32_t index_count, uint32_t first_index);

void mg_vulkan_renderer_push_constants  (mg_vulkan_pipeline_t *program, uint32_t size, void *data);