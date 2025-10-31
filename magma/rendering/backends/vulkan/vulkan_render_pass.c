#include "vulkan_render_pass.h"
#include "vulkan_renderer.h"
#include "vulkan_resources.h"

#include <stdlib.h>
#include <assert.h>

VkRenderPass mg_vulkan_create_render_pass_internal(
    VkFormat color_format, 
    VkFormat depth_format
)
{
    VkAttachmentDescription color_attachment;
    color_attachment.format = color_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference color_ref;
    color_ref.attachment = 0;
    color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_ref;
    depth_ref.attachment = 1;
    depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pColorAttachments = &color_ref;
    subpass.colorAttachmentCount = 1;

    VkAttachmentDescription attachments[2];
    attachments[0] = color_attachment;

    VkRenderPassCreateInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};

    if (depth_format)
    {
        VkAttachmentDescription depth_stencil_attachment;
        depth_stencil_attachment.format = depth_format;
        depth_stencil_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_stencil_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_stencil_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_stencil_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_stencil_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_stencil_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_stencil_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1] = depth_stencil_attachment;

        render_pass_info.attachmentCount = 2;
        subpass.pDepthStencilAttachment = &depth_ref;
    }
    else render_pass_info.attachmentCount = 1;

    VkSubpassDependency deps[2];
    deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    deps[0].dstSubpass = 0;
    deps[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    deps[0].srcAccessMask = 0;
    deps[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    deps[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    deps[1].dstSubpass = 0;
    deps[1].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | 
                           VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    deps[1].srcAccessMask = 0;
    deps[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | 
                          VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    deps[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    render_pass_info.pAttachments = attachments;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 2;
    render_pass_info.pDependencies = deps;

    VkRenderPass render_pass;
    VkResult result = vkCreateRenderPass(vk_ctx.device.handle, &render_pass_info, NULL, &render_pass);
    assert(result == VK_SUCCESS);

    return render_pass;
}

static VkFramebuffer mg_vulkan_create_framebuffer(
    VkRenderPass render_pass,
    VkImageView *attachments,
    uint32_t attachment_count,
    uint32_t width,
    uint32_t height
)
{
    VkFramebufferCreateInfo fb_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    fb_info.renderPass = render_pass;
    fb_info.attachmentCount = attachment_count;
    fb_info.pAttachments = attachments;
    fb_info.width = width;
    fb_info.height = height;
    fb_info.layers = 1;

    VkFramebuffer framebuffer;
    VkResult result = vkCreateFramebuffer(vk_ctx.device.handle, &fb_info, NULL, &framebuffer);
    assert(result == VK_SUCCESS);

    return framebuffer;
}

static void mg_vulkan_begin_render_pass_internal(
    VkRenderPass render_pass,
    VkFramebuffer framebuffer,
    mg_render_pass_begin_info *begin_info
)
{
    VkRenderPassBeginInfo info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    info.renderPass = render_pass;
    info.framebuffer = framebuffer;
    info.renderArea.offset = (VkOffset2D){begin_info->render_area.x, begin_info->render_area.y};
    info.renderArea.extent = (VkExtent2D){begin_info->render_area.z, begin_info->render_area.w};

    VkClearValue clear_values[2] = {0};
    clear_values[0].color = (VkClearColorValue){{
        begin_info->clear_value.r, 
        begin_info->clear_value.g,
        begin_info->clear_value.b, 
        begin_info->clear_value.a
    }};
    clear_values[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0};

    info.clearValueCount = 2;
    info.pClearValues = clear_values;

    vkCmdBeginRenderPass(vk_ctx.command_buffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void mg_vulkan_update_render_pass(mg_vulkan_render_pass *render_pass, 
    mg_render_pass_update_info *resize_info)
{
    VkImageView fb_attachments[2];
    fb_attachments[0] = ((mg_vulkan_image*)resize_info->color_image)->view;

    uint32_t attachment_count;
    if (resize_info->depth_stencil_image)
    {
        fb_attachments[1] = ((mg_vulkan_image*)resize_info->depth_stencil_image)->view;
        attachment_count = 2;
    }
    else attachment_count = 1;

    vkDestroyFramebuffer(vk_ctx.device.handle, render_pass->framebuffer, NULL);
    render_pass->framebuffer = mg_vulkan_create_framebuffer(
        render_pass->render_pass,
        fb_attachments,
        attachment_count,
        resize_info->width,
        resize_info->height
    );
}

mg_vulkan_render_pass *mg_vulkan_create_render_pass(mg_render_pass_create_info *create_info)
{
    mg_vulkan_render_pass *render_pass = (mg_vulkan_render_pass*)malloc(sizeof(mg_vulkan_render_pass));

    VkFormat color_format = (VkFormat)create_info->color_attachment.format;
    VkFormat depth_format =
        create_info->depth_stencil_attachment.image == NULL ?
        VK_FORMAT_UNDEFINED :
        (VkFormat)create_info->depth_stencil_attachment.format;

    render_pass->render_pass = mg_vulkan_create_render_pass_internal(color_format, depth_format);

    VkImageView fb_attachments[2];
    fb_attachments[0] = ((mg_vulkan_image*)create_info->color_attachment.image)->view;

    uint32_t attachment_count;
    if (depth_format)
    {
        fb_attachments[1] = ((mg_vulkan_image*)create_info->depth_stencil_attachment.image)->view;
        attachment_count = 2;
    }
    else attachment_count = 1;

    render_pass->framebuffer = mg_vulkan_create_framebuffer(
        render_pass->render_pass,
        fb_attachments,
        attachment_count,
        create_info->width,
        create_info->height
    );

    return render_pass;
}

void mg_vulkan_destroy_render_pass(mg_vulkan_render_pass *render_pass)
{
    mg_vulkan_free_resource((mg_vulkan_resource){.type = MG_VULKAN_RESOURCE_TYPE_RENDER_PASS, .render_pass = render_pass});
}

void mg_vulkan_begin_render_pass(mg_vulkan_render_pass *render_pass, 
    mg_render_pass_begin_info *begin_info)
{
    mg_vulkan_begin_render_pass_internal(render_pass->render_pass, 
        render_pass->framebuffer, 
        begin_info);
}

void mg_vulkan_begin_default_render_pass(mg_render_pass_begin_info *begin_info)
{
    mg_vulkan_begin_render_pass_internal(vk_ctx.render_pass,
        vk_ctx.swapchain.framebuffers[vk_ctx.image_index],
        begin_info);
}

void mg_vulkan_end_render_pass(void)
{
    vkCmdEndRenderPass(vk_ctx.command_buffer);
}