#pragma once

#include "vulkan_types.inl"
#include "../rhi_renderer.h"

mg_vulkan_pipeline_t   *mg_vulkan_create_pipeline   (mg_pipeline_create_info_t *create_info);
void                    mg_vulkan_destroy_pipeline  (mg_vulkan_pipeline_t *program);
void                    mg_vulkan_bind_pipeline     (mg_vulkan_pipeline_t *program);