#pragma once

#include "vulkan_renderer.h"

uint32_t                            mg_vulkan_find_memory_type                  (uint32_t type_filter, VkMemoryPropertyFlags properties);
void                                mg_vulkan_allocate_buffer                   (size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *memory);
void                                mg_vulkan_copy_buffer                       (VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

mg_vulkan_buffer       *mg_vulkan_create_buffer         (const mg_buffer_create_info *create_info);
void                    mg_vulkan_destroy_buffer        (mg_vulkan_buffer *buffer);
void                    mg_vulkan_update_buffer         (mg_vulkan_buffer *buffer, size_t size, void *data);

void                    mg_vulkan_bind_vertex_buffer    (mg_vulkan_buffer *buffer);
void                    mg_vulkan_bind_index_buffer     (mg_vulkan_buffer *buffer, mg_index_type index_type);