#pragma once

#include "vulkan_types.h"

VkCommandBuffer mg_vulkan_create_command_buffer         (void);

VkCommandBuffer mg_vulkan_begin_single_time_commands    (void);
void            mg_vulkan_end_single_time_commands      (VkCommandBuffer commandBuffer);

void            mg_vulkan_begin_command_buffer          (VkCommandBuffer buffer);
void            mg_vulkan_end_command_buffer            (VkCommandBuffer buffer);
void            mg_vulkan_free_command_buffer           (VkCommandBuffer buffer);

void            mg_vulkan_command_buffer_set_viewport   (VkCommandBuffer buffer, int32_t x, int32_t y, uint32_t width, uint32_t height);
void            mg_vulkan_command_buffer_set_scissor    (VkCommandBuffer buffer, int32_t x, int32_t y, uint32_t width, uint32_t height);