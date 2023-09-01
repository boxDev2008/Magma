#pragma once

#include "vulkan_types.inl"
#include "../llapi_renderer.h"

mg_vulkan_program_t    *mg_vulkan_create_program(mg_program_create_info_t *create_info);
void                    mg_vulkan_destroy_program(mg_vulkan_program_t *program);
void                    mg_vulkan_bind_program(mg_vulkan_program_t *program);