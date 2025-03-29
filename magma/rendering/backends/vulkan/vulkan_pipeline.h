#pragma once

#include "vulkan_types.h"

mg_vulkan_pipeline     *mg_vulkan_create_pipeline   (mg_pipeline_create_info *create_info);
void                    mg_vulkan_destroy_pipeline  (mg_vulkan_pipeline *program);
void                    mg_vulkan_bind_pipeline     (mg_vulkan_pipeline *program);