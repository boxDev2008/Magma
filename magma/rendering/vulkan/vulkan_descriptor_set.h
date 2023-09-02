#pragma once

#include "vulkan_types.inl"

#include "../llapi_renderer.h"

#include <assert.h>

VkDescriptorSetLayout   mg_vulkan_create_descriptor_set_layout  (mg_descriptor_set_layout_create_info_t *create_info);
void                    mg_vulkan_destroy_descriptor_set_layout (VkDescriptorSetLayout descriptor_set_layout);

VkDescriptorSet         mg_vulkan_create_descriptor_set         (mg_descriptor_set_create_info_t *create_info);
void                    mg_vulkan_update_descriptor_set         (VkDescriptorSet descriptor_set, mg_descriptor_write_t *descriptor_write);
void                    mg_vulkan_destroy_descriptor_set        (VkDescriptorSet descriptor_set);

void                    mg_vulkan_bind_descriptor_set           (VkDescriptorSet descriptor_set, mg_vulkan_program_t *program, uint32_t set_index);