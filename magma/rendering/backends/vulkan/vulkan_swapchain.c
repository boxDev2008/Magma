#include "vulkan_renderer.h"
#include "vulkan_image.h"
#include "vulkan_types.h"

#include "math/math.h"

#include <stdlib.h>
#include <assert.h>

VkExtent2D mg_vulkan_choose_swap_extent(const VkSurfaceCapabilitiesKHR *capabilities, int32_t width, int32_t height)
{
    if (capabilities->currentExtent.width != UINT32_MAX)
        return capabilities->currentExtent;
    else
    {
        VkExtent2D actual_extent = {width, height};
        actual_extent.width = mg_math_clampf(actual_extent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
        actual_extent.height = mg_math_clampf(actual_extent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);
        return actual_extent;
    }
}

void mg_vulkan_create_swapchain(mg_swapchain_config_info *config_info)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_context.physical_device.handle, vk_context.surface, &capabilities);

    VkExtent2D extent = mg_vulkan_choose_swap_extent(&capabilities, config_info->width, config_info->height);

    uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
        image_count = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    create_info.surface = vk_context.surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = config_info->format;
    create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = NULL;
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = config_info->present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = vk_context.swapchain.handle;

    VkResult result = vkCreateSwapchainKHR(vk_context.device.handle, &create_info, NULL, &vk_context.swapchain.handle);
    assert(result == VK_SUCCESS);

	if (create_info.oldSwapchain)
    	vkDestroySwapchainKHR(vk_context.device.handle, create_info.oldSwapchain, NULL);

    vkGetSwapchainImagesKHR(vk_context.device.handle, vk_context.swapchain.handle, &image_count, NULL);
    vk_context.swapchain.image_count = image_count;

    vk_context.swapchain.images = (VkImage*) realloc(vk_context.swapchain.images, image_count * sizeof(VkImage));
    vkGetSwapchainImagesKHR(vk_context.device.handle, vk_context.swapchain.handle, &image_count, vk_context.swapchain.images);

    vk_context.swapchain.image_views = (VkImageView*) realloc(vk_context.swapchain.image_views, image_count * sizeof(VkImageView));

	uint32_t i;
    for (i = 0; i < image_count; i++)
    {
        VkImageViewCreateInfo image_view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        image_view_create_info.image = vk_context.swapchain.images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = config_info->format;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        result = vkCreateImageView(vk_context.device.handle, &image_view_create_info, NULL, &vk_context.swapchain.image_views[i]);
        assert(result == VK_SUCCESS);
    }

    mg_vulkan_allocate_image(config_info->width, config_info->height,
		VK_IMAGE_TYPE_2D, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&vk_context.swapchain.depth_image, &vk_context.swapchain.depth_image_memory);

    VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_info.image = vk_context.swapchain.depth_image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = VK_FORMAT_D32_SFLOAT;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    result = vkCreateImageView(vk_context.device.handle, &view_info, NULL, &vk_context.swapchain.depth_image_view);
    assert(result == VK_SUCCESS);

    vk_context.swapchain.framebuffers = (VkFramebuffer*) realloc(vk_context.swapchain.framebuffers, image_count * sizeof(VkFramebuffer));
    for (i = 0; i < image_count; i++)
    {
        VkFramebufferCreateInfo framebuffer_create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        framebuffer_create_info.attachmentCount = 2;
        framebuffer_create_info.renderPass = vk_context.render_pass;
        VkImageView attachments[2] = { vk_context.swapchain.image_views[i], vk_context.swapchain.depth_image_view };
        framebuffer_create_info.pAttachments = attachments;
        framebuffer_create_info.width = extent.width;
        framebuffer_create_info.height = extent.height;
        framebuffer_create_info.layers = 1;

        result = vkCreateFramebuffer(vk_context.device.handle, &framebuffer_create_info, NULL, &vk_context.swapchain.framebuffers[i]);
        assert(result == VK_SUCCESS);
    }
}

void mg_vulkan_cleanup_swapchain(void)
{
    for (uint32_t i = 0; i < vk_context.swapchain.image_count; i++)
    {
        vkDestroyFramebuffer(vk_context.device.handle, vk_context.swapchain.framebuffers[i], NULL);
        vkDestroyImageView(vk_context.device.handle, vk_context.swapchain.image_views[i], NULL);
    }

    vkDestroyImageView(vk_context.device.handle, vk_context.swapchain.depth_image_view, NULL);
    vkDestroyImage(vk_context.device.handle, vk_context.swapchain.depth_image, NULL);
    vkFreeMemory(vk_context.device.handle, vk_context.swapchain.depth_image_memory, NULL);

    vkDestroySwapchainKHR(vk_context.device.handle, vk_context.swapchain.handle, NULL);

    free(vk_context.swapchain.framebuffers);
    free(vk_context.swapchain.image_views);
    free(vk_context.swapchain.images);
}

void mg_vulkan_configure_swapchain(mg_swapchain_config_info *config_info)
{
    vkDeviceWaitIdle(vk_context.device.handle);

    for (uint32_t i = 0; i < vk_context.swapchain.image_count; i++)
    {
        vkDestroyFramebuffer(vk_context.device.handle, vk_context.swapchain.framebuffers[i], NULL);
        vkDestroyImageView(vk_context.device.handle, vk_context.swapchain.image_views[i], NULL);
    }

    vkDestroyImageView(vk_context.device.handle, vk_context.swapchain.depth_image_view, NULL);
    vkDestroyImage(vk_context.device.handle, vk_context.swapchain.depth_image, NULL);
    vkFreeMemory(vk_context.device.handle, vk_context.swapchain.depth_image_memory, NULL);

    mg_vulkan_create_swapchain(config_info);
}