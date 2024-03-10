#include "vulkan_render_pass.h"
#include "vulkan_renderer.h"

#include <stdlib.h>
#include <assert.h>

VkRenderPass mg_vulkan_create_render_pass(mg_render_pass_create_info_t *create_info)
{
    VkAttachmentDescription color_attachment = { 0 };
    color_attachment.format = create_info->color_attachment.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = MG_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = MG_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depth_attachment = { 0 };

    VkAttachmentReference color_attachment_ref;
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref;
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = { 0 };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.colorAttachmentCount = 1;

    VkRenderPassCreateInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};

    if (create_info->has_depth_stencil_attachment)
    {
        depth_attachment.format = create_info->depth_stencil_attachment.format;
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        render_pass_info.pAttachments = (VkAttachmentDescription[]){ color_attachment, depth_attachment };
        render_pass_info.attachmentCount = 2;

        subpass.pDepthStencilAttachment = &depth_attachment_ref;
    }
    else
    {
        render_pass_info.pAttachments = &color_attachment;
        render_pass_info.attachmentCount = 1;
    }

    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    VkSubpassDependency dependenies[2];
    dependenies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependenies[0].dstSubpass = 0;
    dependenies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependenies[0].srcAccessMask = 0;
    dependenies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependenies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    dependenies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependenies[1].dstSubpass = 0;
    dependenies[1].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependenies[1].srcAccessMask = 0;
    dependenies[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependenies[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    render_pass_info.dependencyCount = 2;
    render_pass_info.pDependencies = dependenies;

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

    VkClearValue clear_values[2];
    clear_values[0].color = (VkClearColorValue){{begin_info->clear_value.r, begin_info->clear_value.g, begin_info->clear_value.b, begin_info->clear_value.a}};
    clear_values[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    render_pass_info.clearValueCount = 2;
    render_pass_info.pClearValues = clear_values;

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