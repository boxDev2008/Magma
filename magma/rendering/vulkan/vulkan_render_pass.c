#include "vulkan_render_pass.h"
#include "vulkan_renderer.h"

#include <stdlib.h>
#include <assert.h>

VkRenderPass mg_vulkan_create_render_pass(mg_render_pass_create_info_t *create_info)
{
    VkAttachmentDescription color_attachment = { 0 };

    color_attachment.format = create_info->color_attachment.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        
    color_attachment.loadOp = create_info->color_attachment.load_op;
    color_attachment.storeOp = create_info->color_attachment.store_op;

    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference color_attachment_ref;
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = { 0 };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                               VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VkRenderPass render_pass;

    VkResult result = vkCreateRenderPass(vulkan_context.device.handle, &render_pass_info, NULL, &render_pass);
    assert(result == VK_SUCCESS);

    return render_pass;
}

void mg_vulkan_destroy_render_pass(VkRenderPass render_pass)
{
    vkDestroyRenderPass(vulkan_context.device.handle, render_pass, NULL);
}

void mg_vulkan_begin_render_pass(VkRenderPass render_pass, VkFramebuffer framebuffer, mg_render_pass_begin_info_t *begin_info)
{
    VkRenderPassBeginInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    render_pass_info.renderPass = render_pass;
    render_pass_info.framebuffer = framebuffer;
    
    render_pass_info.renderArea.offset = (VkOffset2D){ begin_info->render_area.x, begin_info->render_area.y };
    render_pass_info.renderArea.extent = (VkExtent2D){ begin_info->render_area.z, begin_info->render_area.w };

    VkClearValue clearColor = {{{begin_info->clear_value.r, begin_info->clear_value.g, begin_info->clear_value.b, begin_info->clear_value.a}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(vulkan_context.command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void mg_vulkan_begin_default_render_pass(mg_render_pass_begin_info_t *begin_info)
{
    mg_vulkan_begin_render_pass(vulkan_context.render_pass,
        vulkan_context.swapchain.framebuffers[vulkan_context.image_index], begin_info);
}

void mg_vulkan_end_render_pass(void)
{
    vkCmdEndRenderPass(vulkan_context.command_buffer);
}