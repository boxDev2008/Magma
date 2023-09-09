#pragma once

#include "vulkan_types.inl"

VkCommandBuffer mg_vulkan_create_command_buffer         (void);
void            mg_vulkan_submit_command_buffer         (VkCommandBuffer buffer);

VkCommandBuffer mg_vulkan_begin_single_time_commands    (void);
void            mg_vulkan_end_single_time_commands      (VkCommandBuffer commandBuffer);

void            mg_vulkan_begin_command_buffer          (VkCommandBuffer buffer);
void            mg_vulkan_end_command_buffer            (VkCommandBuffer buffer);
void            mg_vulkan_free_command_buffer           (VkCommandBuffer buffer);

void            mg_vulkan_command_buffer_set_viewport   (VkCommandBuffer buffer, uint32_t width, uint32_t height);