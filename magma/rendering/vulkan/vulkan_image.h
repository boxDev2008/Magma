#pragma once

#include "vulkan_types.inl"

void                mg_vulkan_transition_image_layout   (VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
void                mg_vulkan_copy_buffer_to_image      (VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

void                mg_vulkan_allocate_image            (uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *memory);

mg_vulkan_texture_image_t  *mg_vulkan_create_texture_image  (mg_texture_image_create_info_t *create_info);
void                        mg_vulkan_write_texture_image   (mg_vulkan_texture_image_t *texture_image, mg_texture_image_write_info_t *write_info);
void                        mg_vulkan_destroy_texture_image (mg_vulkan_texture_image_t *texture_image);

VkImageView                 mg_vulkan_create_texture_view   (mg_texture_view_create_info_t *create_info);
void                        mg_vulkan_destroy_texture_view  (VkImageView texture_view);

VkSampler                   mg_vulkan_create_sampler        (mg_sampler_create_info_t *create_info);
void                        mg_vulkan_destroy_sampler       (VkSampler sampler);

VkFramebuffer               mg_vulkan_create_framebuffer    (mg_framebuffer_create_info_t *create_info);
void                        mg_vulkan_destroy_framebuffer   (VkFramebuffer framebuffer);