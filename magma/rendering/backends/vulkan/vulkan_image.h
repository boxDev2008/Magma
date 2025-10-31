#pragma once

#include "vulkan_types.h"

void                mg_vulkan_transition_image_layout   (VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
void                mg_vulkan_copy_buffer_to_image      (VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
void                mg_vulkan_allocate_image            (uint32_t width, uint32_t height, VkImageType type, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *memory);

mg_vulkan_image   *mg_vulkan_create_image               (mg_image_create_info *create_info);
void                mg_vulkan_destroy_image             (mg_vulkan_image *image);
void                mg_vulkan_update_image              (mg_vulkan_image *image, mg_image_write_info *write_info);

VkDescriptorSet     mg_vulkan_create_image_array  (void);
void                mg_vulkan_destroy_image_array (VkDescriptorSet set);
void                mg_vulkan_update_image_array  (VkDescriptorSet set, mg_vulkan_image **images, VkSampler *samplers, uint32_t count);
void                mg_vulkan_bind_image_array    (VkDescriptorSet set);

VkSampler           mg_vulkan_create_sampler            (mg_sampler_create_info *create_info);
void                mg_vulkan_destroy_sampler           (VkSampler sampler);