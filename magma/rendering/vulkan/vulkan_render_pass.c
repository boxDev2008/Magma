#include "vulkan_render_pass.h"
#include "vulkan_renderer.h"

#include <assert.h>

VkRenderPass mg_vulkan_create_render_pass(void)
{
    // TODO (box): Make it customizable

    VkAttachmentDescription color_attachment = { 0 };
    color_attachment.format = VK_FORMAT_B8G8R8A8_SRGB;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = { 0 };
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = { 0 };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_passInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_passInfo.attachmentCount = 1;
    render_passInfo.pAttachments = &color_attachment;
    render_passInfo.subpassCount = 1;
    render_passInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency;

	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    render_passInfo.dependencyCount = 1;
    render_passInfo.pDependencies = &dependency;

    VkRenderPass render_pass;

    VkResult result = vkCreateRenderPass(context.device.handle, &render_passInfo, NULL, &render_pass);
    assert(result == VK_SUCCESS);

    return render_pass;
}

void mg_vulkan_destroy_render_pass(VkRenderPass render_pass)
{
    vkDestroyRenderPass(context.device.handle, render_pass, NULL);
}

void mg_vulkan_begin_render_pass(VkRenderPass render_pass, mg_render_pass_begin_info_t *begin_info)
{
    VkRenderPassBeginInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    render_pass_info.renderPass = render_pass;
    render_pass_info.framebuffer = begin_info->framebuffer.internal_data;
    
    render_pass_info.renderArea.offset = (VkOffset2D){ begin_info->render_area.x, begin_info->render_area.y };
    render_pass_info.renderArea.extent = (VkExtent2D){ begin_info->render_area.z, begin_info->render_area.w };

    VkClearValue clearColor = {{{begin_info->clear_value.r, begin_info->clear_value.g, begin_info->clear_value.b, begin_info->clear_value.a}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(context.command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void mg_vulkan_end_render_pass(void)
{
    vkCmdEndRenderPass(context.command_buffer);
}