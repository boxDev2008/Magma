#pragma once

#include "vulkan_types.h"

VkDescriptorSet mg_vulkan_commit_image_descriptor_cache(mg_vulkan_descriptor_cache* cache);

void mg_vulkan_descriptor_cache_invalidate_image_view(mg_vulkan_descriptor_cache* cache, VkImageView imag_view);
void mg_vulkan_descriptor_cache_invalidate_sampler(mg_vulkan_descriptor_cache* cache, VkSampler sampler);;