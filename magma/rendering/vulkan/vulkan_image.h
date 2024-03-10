#pragma once

#include "vulkan_types.h"

void                mg_vulkan_transition_image_layout   (VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
void                mg_vulkan_copy_buffer_to_image      (VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
void                mg_vulkan_allocate_image            (uint32_t width, uint32_t height, VkImageType type, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *memory);

mg_vulkan_image_t   *mg_vulkan_create_image             (mg_image_create_info_t *create_info);
void                mg_vulkan_destroy_image             (mg_vulkan_image_t *image);
void                mg_vulkan_write_image               (mg_vulkan_image_t *image, mg_image_write_info_t *write_info);
void                mg_vulkan_update_image              (mg_vulkan_image_t *image, VkSampler sampler, uint32_t binding);
void                mg_vulkan_bind_image                (mg_vulkan_image_t *image, mg_vulkan_pipeline_t *pipeline);

VkSampler           mg_vulkan_create_sampler            (mg_sampler_create_info_t *create_info);
void                mg_vulkan_destroy_sampler           (VkSampler sampler);

VkFramebuffer       mg_vulkan_create_framebuffer        (mg_framebuffer_create_info_t *create_info);
void                mg_vulkan_destroy_framebuffer       (VkFramebuffer framebuffer);