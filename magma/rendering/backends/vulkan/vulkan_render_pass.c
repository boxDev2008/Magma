#include "vulkan_render_pass.h"
#include "vulkan_renderer.h"
#include "vulkan_resources.h"
#include "vulkan_command_buffer.h"

#include <stdlib.h>
#include <assert.h>

VkRenderPass mg_vulkan_create_render_pass_internal(
    VkFormat color_format,
    VkFormat depth_format
)
{
    VkAttachmentDescription attachments[2];
    VkAttachmentReference color_ref = {0};
    VkAttachmentReference depth_ref = {0};
    uint32_t attachment_count = 0;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    VkSubpassDependency deps[2] = {0};
    uint32_t dep_count = 0;

    const bool is_default = !vk_ctx.default_render_pass;
    if (color_format != VK_FORMAT_UNDEFINED)
    {
        VkAttachmentDescription color_attachment = {0};
        color_attachment.format = color_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout =
            is_default ?
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR :
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        color_ref.attachment = attachment_count;
        color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        attachments[attachment_count++] = color_attachment;
        subpass.pColorAttachments = &color_ref;
        subpass.colorAttachmentCount = 1;

        deps[dep_count].srcSubpass = VK_SUBPASS_EXTERNAL;
        deps[dep_count].dstSubpass = 0;
        deps[dep_count].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        deps[dep_count].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        deps[dep_count].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dep_count++;
    }

    if (depth_format != VK_FORMAT_UNDEFINED)
    {
        VkAttachmentDescription depth_attachment = {0};
        depth_attachment.format = depth_format;
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout =
            is_default ?
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL :
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

        depth_ref.attachment = attachment_count;
        depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        attachments[attachment_count++] = depth_attachment;
        subpass.pDepthStencilAttachment = &depth_ref;

        deps[dep_count].srcSubpass = VK_SUBPASS_EXTERNAL;
        deps[dep_count].dstSubpass = 0;
        deps[dep_count].srcStageMask = 
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        deps[dep_count].dstStageMask = 
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        deps[dep_count].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dep_count++;
    }

    VkRenderPassCreateInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_pass_info.attachmentCount = attachment_count;
    render_pass_info.pAttachments = attachment_count ? attachments : NULL;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = dep_count;
    render_pass_info.pDependencies = dep_count ? deps : NULL;

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
    const mg_render_pass_bind_info *begin_info
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
    mg_vulkan_command_buffer_set_viewport(
        vk_ctx.command_buffer,
        begin_info->render_area.x,
        begin_info->render_area.y,
        begin_info->render_area.z,
        begin_info->render_area.w
    );
    mg_vulkan_command_buffer_set_scissor(
        vk_ctx.command_buffer,
        begin_info->render_area.x,
        begin_info->render_area.y,
        begin_info->render_area.z,
        begin_info->render_area.w
    );
}

void mg_vulkan_update_render_pass(
    mg_vulkan_render_pass *render_pass, 
    const mg_render_pass_update_info *update_info)
{
    VkImageView fb_attachments[2];
    uint32_t attachment_count = 0;

    if (update_info->color_image)
    {
        fb_attachments[attachment_count++] =
            ((mg_vulkan_image*)update_info->color_image)->view;
    }

    if (update_info->depth_stencil_image)
    {
        fb_attachments[attachment_count++] =
            ((mg_vulkan_image*)update_info->depth_stencil_image)->view;
    }

    vkDestroyFramebuffer(vk_ctx.device.handle, render_pass->framebuffer, NULL);

    render_pass->framebuffer = mg_vulkan_create_framebuffer(
        render_pass->render_pass,
        fb_attachments,
        attachment_count,
        update_info->width,
        update_info->height
    );
}

mg_vulkan_render_pass *mg_vulkan_create_render_pass(const mg_render_pass_create_info *create_info)
{
    mg_vulkan_render_pass *render_pass =
        (mg_vulkan_render_pass*)malloc(sizeof(mg_vulkan_render_pass));

    render_pass->render_pass =
        mg_vulkan_create_render_pass_internal(
            create_info->color_attachment.format,
            create_info->depth_stencil_attachment.format
        );

    VkImageView fb_attachments[2];
    uint32_t attachment_count = 0;

    if (create_info->color_attachment.image)
    {
        fb_attachments[attachment_count++] =
            ((mg_vulkan_image*)create_info->color_attachment.image)->view;
    }

    if (create_info->depth_stencil_attachment.image)
    {
        fb_attachments[attachment_count++] =
            ((mg_vulkan_image*)create_info->depth_stencil_attachment.image)->view;
    }

    render_pass->framebuffer = mg_vulkan_create_framebuffer(
        render_pass->render_pass,
        attachment_count ? fb_attachments : NULL,
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

void mg_vulkan_bind_render_pass(mg_vulkan_render_pass *render_pass, 
    const mg_render_pass_bind_info *begin_info)
{
    if (vk_ctx.inside_render_pass)
        vkCmdEndRenderPass(vk_ctx.command_buffer);
    else vk_ctx.inside_render_pass = true;

    if (render_pass)
        mg_vulkan_begin_render_pass_internal(
            render_pass->render_pass, 
            render_pass->framebuffer, 
            begin_info);
    else
        mg_vulkan_begin_render_pass_internal(
            vk_ctx.default_render_pass, 
            vk_ctx.swapchain.framebuffers[vk_ctx.swapchain.image_index], 
            begin_info);
}