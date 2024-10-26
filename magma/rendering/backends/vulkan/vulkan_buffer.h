#pragma once

#include "vulkan_renderer.h"

uint32_t                            mg_vulkan_find_memory_type                  (uint32_t type_filter, VkMemoryPropertyFlags properties);
void                                mg_vulkan_allocate_buffer                   (size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *memory);
void                                mg_vulkan_copy_buffer                       (VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

mg_vulkan_vertex_buffer          *mg_vulkan_create_vertex_buffer              (size_t size, void *data);
void                                mg_vulkan_destroy_vertex_buffer             (mg_vulkan_vertex_buffer *buffer);

mg_vulkan_index_buffer           *mg_vulkan_create_index_buffer               (size_t size, void *data);
void                                mg_vulkan_destroy_index_buffer              (mg_vulkan_index_buffer *buffer);

mg_vulkan_dynamic_vertex_buffer  *mg_vulkan_create_dynamic_vertex_buffer      (size_t size);
void                                mg_vulkan_destroy_dynamic_vertex_buffer     (mg_vulkan_dynamic_vertex_buffer *buffer);
void                                mg_vulkan_update_dynamic_vertex_buffer      (mg_vulkan_dynamic_vertex_buffer *buffer, size_t size, void *data);

mg_vulkan_dynamic_index_buffer   *mg_vulkan_create_dynamic_index_buffer       (size_t size);
void                                mg_vulkan_destroy_dynamic_index_buffer      (mg_vulkan_dynamic_index_buffer *buffer);
void                                mg_vulkan_update_dynamic_index_buffer       (mg_vulkan_dynamic_index_buffer *buffer, size_t size, void *data);

mg_vulkan_uniform_buffer         *mg_vulkan_create_uniform_buffer             (size_t size);
void                                mg_vulkan_destroy_uniform_buffer            (mg_vulkan_uniform_buffer *buffer);
void                                mg_vulkan_update_uniform_buffer             (mg_vulkan_uniform_buffer *buffer, size_t size, void *data);

void                                mg_vulkan_bind_vertex_buffer                (mg_vulkan_vertex_buffer *buffer);
void                                mg_vulkan_bind_dynamic_vertex_buffer        (mg_vulkan_dynamic_vertex_buffer *buffer);

void                                mg_vulkan_bind_index_buffer                 (mg_vulkan_index_buffer *buffer, mg_index_type index_type);
void                                mg_vulkan_bind_dynamic_index_buffer         (mg_vulkan_dynamic_index_buffer *buffer, mg_index_type index_type);

void                                mg_vulkan_bind_unifom_buffer                (mg_vulkan_uniform_buffer *buffer);