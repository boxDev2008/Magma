#pragma once

#include "vulkan_types.h"

#if MG_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
    #define MG_VULKAN_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif MG_PLATFORM_LINUX
    #define VK_USE_PLATFORM_XLIB_KHR
    #define VK_USE_PLATFORM_XCB_KHR
    #define MG_VULKAN_SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif

extern mg_vulkan_context vk_context;

void mg_vulkan_renderer_initialize      (mg_lowl_renderer_init_info *init_info);
void mg_vulkan_renderer_shutdown        (void);

void mg_vulkan_renderer_begin_frame     (void);
void mg_vulkan_renderer_end_frame       (void);

void mg_vulkan_renderer_present_frame   (void);

void mg_vulkan_renderer_wait            (void);

void mg_vulkan_renderer_viewport        (int32_t x, int32_t y, uint32_t width, uint32_t height);
void mg_vulkan_renderer_scissor         (int32_t x, int32_t y, uint32_t width, uint32_t height);

void mg_vulkan_renderer_draw            (uint32_t vertex_count, uint32_t first_vertex);
void mg_vulkan_renderer_draw_indexed    (uint32_t index_count, uint32_t first_index);

void mg_vulkan_renderer_push_constants  (uint32_t size, void *data);