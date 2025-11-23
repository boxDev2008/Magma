#pragma once

#include "vulkan_types.h"

VkRenderPass    mg_vulkan_create_render_pass_internal(VkFormat color_format, VkFormat depth_format);

mg_vulkan_render_pass *mg_vulkan_create_render_pass  (const mg_render_pass_create_info *create_info);
void            mg_vulkan_destroy_render_pass       (mg_vulkan_render_pass *render_pass);
void            mg_vulkan_update_render_pass        (mg_vulkan_render_pass *render_pass, const mg_render_pass_update_info *update_info);

void mg_vulkan_bind_render_pass(mg_vulkan_render_pass *render_pass, const mg_render_pass_bind_info *begin_info);