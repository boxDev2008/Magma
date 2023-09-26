#pragma once

#include "vulkan_types.inl"
#include "vulkan_renderer.h"

#include "../rhi_renderer.h"

uint32_t                            mg_vulkan_find_memory_type                  (uint32_t type_filter, VkMemoryPropertyFlags properties);
void                                mg_vulkan_allocate_buffer                   (size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *memory);
void                                mg_vulkan_copy_buffer                       (VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

mg_vulkan_vertex_buffer_t          *mg_vulkan_create_vertex_buffer              (size_t size, void *data);
void                                mg_vulkan_destroy_vertex_buffer             (mg_vulkan_vertex_buffer_t *buffer);

mg_vulkan_index_buffer_t           *mg_vulkan_create_index_buffer               (size_t size, void *data);
void                                mg_vulkan_destroy_index_buffer              (mg_vulkan_index_buffer_t *buffer);

mg_vulkan_dynamic_vertex_buffer_t  *mg_vulkan_create_dynamic_vertex_buffer      (size_t size);
void                                mg_vulkan_destroy_dynamic_vertex_buffer     (mg_vulkan_dynamic_vertex_buffer_t *buffer);
void                                mg_vulkan_update_dynamic_vertex_buffer      (mg_vulkan_dynamic_vertex_buffer_t *buffer, size_t size, void *data);

mg_vulkan_dynamic_index_buffer_t   *mg_vulkan_create_dynamic_index_buffer       (size_t size);
void                                mg_vulkan_destroy_dynamic_index_buffer      (mg_vulkan_dynamic_index_buffer_t *buffer);
void                                mg_vulkan_update_dynamic_index_buffer       (mg_vulkan_dynamic_index_buffer_t *buffer, size_t size, void *data);

mg_vulkan_uniform_buffer_t         *mg_vulkan_create_uniform_buffer             (size_t size);
void                                mg_vulkan_destroy_uniform_buffer            (mg_vulkan_uniform_buffer_t *buffer);
void                                mg_vulkan_update_uniform_buffer             (mg_vulkan_uniform_buffer_t *buffer, size_t size, void *data);

void                                mg_vulkan_bind_vertex_buffer                (mg_vulkan_vertex_buffer_t *buffer);
void                                mg_vulkan_bind_dynamic_vertex_buffer        (mg_vulkan_dynamic_vertex_buffer_t *buffer);

void                                mg_vulkan_bind_index_buffer                 (mg_vulkan_index_buffer_t *buffer, mg_index_type_t index_type);
void                                mg_vulkan_bind_dynamic_index_buffer         (mg_vulkan_dynamic_index_buffer_t *buffer, mg_index_type_t index_type);