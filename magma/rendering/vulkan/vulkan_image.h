#pragma once

#include "vulkan_types.inl"

mg_vulkan_image_t   mg_vulkan_create_image              (uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
void                mg_vulkan_destroy_image             (mg_vulkan_image_t *image);

void                mg_vulkan_transition_image_layout   (VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
void                mg_vulkan_copy_buffer_to_image      (VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);