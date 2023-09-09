#include "vulkan_renderer.h"
#include "vulkan_types.inl"

#include "math/math.h"

#include <stdlib.h>
#include <assert.h>

VkSurfaceFormatKHR mg_vulkan_choose_swap_surface_format(const VkSurfaceFormatKHR *available_formats, uint32_t available_format_count)
{
    for (uint32_t i = 0; i < available_format_count; i++)
        if (available_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && available_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return available_formats[i];

    return available_formats[0];
}

VkExtent2D mg_vulkan_choose_swap_extent(const VkSurfaceCapabilitiesKHR *capabilities, int32_t width, int32_t height)
{
    if (capabilities->currentExtent.width != UINT32_MAX)
        return capabilities->currentExtent;
    else
    {
        VkExtent2D actual_extent = {
            width,
            height
        };

        actual_extent.width = mg_math_clamp(actual_extent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
        actual_extent.height = mg_math_clamp(actual_extent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

        return actual_extent;
    }
}

void mg_vulkan_create_swapchain(mg_swapchain_config_info_t *config_info)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.physical_device.handle, context.surface, &capabilities);

    /*uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.physical_device.handle, context.surface, &format_count, NULL);

    VkSurfaceFormatKHR *formats = (VkSurfaceFormatKHR*)malloc(format_count * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.physical_device.handle, context.surface, &format_count, formats);
    VkSurfaceFormatKHR surface_format = mg_vulkan_choose_swap_surface_format(formats, format_count);
    free(formats);*/

    VkExtent2D extent = mg_vulkan_choose_swap_extent(&capabilities, config_info->extent.x, config_info->extent.y);

    context.swapchain.image_count = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && context.swapchain.image_count > capabilities.maxImageCount)
        context.swapchain.image_count = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    create_info.surface = context.surface;
    create_info.minImageCount = context.swapchain.image_count;

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
    
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(context.device.handle, &create_info, NULL, &context.swapchain.handle);
    assert(result == VK_SUCCESS);

    vkGetSwapchainImagesKHR(context.device.handle, context.swapchain.handle, &context.swapchain.image_count, NULL);
    context.swapchain.images = (VkImage*)malloc(context.swapchain.image_count * sizeof(VkImage));
    vkGetSwapchainImagesKHR(context.device.handle, context.swapchain.handle, &context.swapchain.image_count, context.swapchain.images);

    context.swapchain.image_views = (VkImageView*)malloc(context.swapchain.image_count * sizeof(VkImageView));

    for (uint32_t i = 0; i < context.swapchain.image_count; i++)
    {
        VkImageViewCreateInfo image_view_create_info = { 0 };
        image_view_create_info.flags = 0;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = context.swapchain.images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = config_info->format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        result = vkCreateImageView(context.device.handle, &image_view_create_info, NULL, &context.swapchain.image_views[i]);
        assert(result == VK_SUCCESS);
    }

    context.swapchain.framebuffers = (VkFramebuffer*)malloc(context.swapchain.image_count * sizeof(VkFramebuffer));
    for (uint32_t i = 0; i < context.swapchain.image_count; i++)
    {
        VkFramebufferCreateInfo framebuffer_create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = &context.swapchain.image_views[i];
        framebuffer_create_info.width = extent.width;
        framebuffer_create_info.height = extent.height;
        framebuffer_create_info.layers = 1;

        VkResult result = vkCreateFramebuffer(context.device.handle, &framebuffer_create_info, NULL, &context.swapchain.framebuffers[i]);
        assert(result == VK_SUCCESS);
    }
}

void mg_vulkan_cleanup_swapchain(void)
{    
    for (uint32_t i = 0; i < context.swapchain.image_count; i++)
    {
        vkDestroyFramebuffer(context.device.handle, context.swapchain.framebuffers[i], NULL);
        vkDestroyImageView(context.device.handle, context.swapchain.image_views[i], NULL);
    }

    free(context.swapchain.framebuffers);
    free(context.swapchain.image_views);

    vkDestroySwapchainKHR(context.device.handle, context.swapchain.handle, NULL);

    free(context.swapchain.images);
}

void mg_vulkan_configure_swapchain(mg_swapchain_config_info_t *config_info)
{
    vkDeviceWaitIdle(context.device.handle);
    
    mg_vulkan_cleanup_swapchain();
    mg_vulkan_create_swapchain(config_info);
}

VkFramebuffer mg_vulkan_get_swapchain_framebuffer(void)
{
    return context.swapchain.framebuffers[context.image_index];
}