#pragma once

#include "vulkan_types.h"

VkRenderPass    mg_vulkan_create_render_pass        (mg_render_pass_create_info *create_info);
void            mg_vulkan_destroy_render_pass       (VkRenderPass render_pass);

void            mg_vulkan_begin_render_pass         (VkRenderPass render_pass, VkFramebuffer framebuffer, mg_render_pass_begin_info *begin_info);
void            mg_vulkan_begin_default_render_pass (mg_render_pass_begin_info *begin_info);

void            mg_vulkan_end_render_pass           (void);