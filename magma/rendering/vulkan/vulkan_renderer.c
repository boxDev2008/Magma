#include "vulkan_renderer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_buffer.h"
#include "vulkan_image.h"

#include "platform/vulkan_platform.h"

#include "math/math.h"

#include <stdlib.h>
#include <assert.h>

mg_vulkan_context_t context;

void mg_vulkan_create_instance(void)
{
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pApplicationName = NULL;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Magma Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    const char *instance_extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        MG_VULKA_SURFACE_EXTENSION_NAME
    };

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 2;
    create_info.ppEnabledExtensionNames = instance_extensions;

    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&create_info, NULL, &context.instance);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_get_physical_device(void)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(context.instance, &device_count, NULL);
    assert(device_count != 0);

    VkPhysicalDevice *devices = (VkPhysicalDevice*)malloc(device_count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(context.instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
        vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            deviceFeatures.geometryShader)
        {
            context.physical_device.handle = devices[i];
            break;
        }
    }

    free(devices);

    context.physical_device.handle_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(context.physical_device.handle, &context.physical_device.handle_props);

    context.physical_device.features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vkGetPhysicalDeviceFeatures2(context.physical_device.handle, &context.physical_device.features);

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(context.physical_device.handle, &queue_family_count, NULL);

    VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties*)malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(context.physical_device.handle, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++)
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            context.physical_device.graphics_family = i;
    
    free(queue_families);
}

void mg_vulkan_create_device(void)
{
    VkDeviceQueueCreateInfo queueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queueCreateInfo.queueFamilyIndex = context.physical_device.graphics_family;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures device_features = { 0 };

    VkDeviceCreateInfo create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};

    create_info.pQueueCreateInfos = &queueCreateInfo;
    create_info.queueCreateInfoCount = 1;

    create_info.pEnabledFeatures = &device_features;

    create_info.enabledLayerCount = 0;

    const char *device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    create_info.enabledExtensionCount = 1;
    create_info.ppEnabledExtensionNames = device_extensions;

    VkResult result = vkCreateDevice(context.physical_device.handle, &create_info, NULL, &context.device.handle);
    assert(result == VK_SUCCESS);

    vkGetDeviceQueue(context.device.handle, context.physical_device.graphics_family, 0, &context.device.graphics_queue);
}

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

void mg_vulkan_create_swapchain(int32_t width, int32_t height)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.physical_device.handle, context.surface, &capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.physical_device.handle, context.surface, &format_count, NULL);

    VkSurfaceFormatKHR *formats = (VkSurfaceFormatKHR*)malloc(format_count * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.physical_device.handle, context.surface, &format_count, formats);
    VkSurfaceFormatKHR surface_format = mg_vulkan_choose_swap_surface_format(formats, format_count);
    free(formats);

    VkExtent2D extent = mg_vulkan_choose_swap_extent(&capabilities, width, height);

    context.swapchain.image_count = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && context.swapchain.image_count > capabilities.maxImageCount)
        context.swapchain.image_count = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    create_info.surface = context.surface;
    create_info.minImageCount = context.swapchain.image_count;

    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;

    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = NULL;

    create_info.preTransform = capabilities.currentTransform;

    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    //create_info.presentMode = context_settings.enable_vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
    create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    create_info.clipped = VK_TRUE;
    
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(context.device.handle, &create_info, NULL, &context.swapchain.handle);
    assert(result == VK_SUCCESS);

    vkGetSwapchainImagesKHR(context.device.handle, context.swapchain.handle, &context.swapchain.image_count, NULL);
    context.swapchain.images = (VkImage*)malloc(context.swapchain.image_count * sizeof(VkImage));
    vkGetSwapchainImagesKHR(context.device.handle, context.swapchain.handle, &context.swapchain.image_count, context.swapchain.images);

    context.swapchain.image_format = surface_format.format;
    context.swapchain.extent = extent;

    context.swapchain.image_views = (VkImageView*)malloc(context.swapchain.image_count * sizeof(VkImageView));

    for (uint32_t i = 0; i < context.swapchain.image_count; i++)
    {
        VkImageViewCreateInfo image_view_create_info = { 0 };
        image_view_create_info.flags = 0;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = context.swapchain.images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = context.swapchain.image_format;
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
}

void mg_vulkan_create_render_pass(void)
{
    VkAttachmentDescription colorAttachment = { 0 };
    colorAttachment.format = context.swapchain.image_format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = { 0 };
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = { 0 };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkSubpassDependency dependency = { 0 };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(context.device.handle, &renderPassInfo, NULL, &context.render_pass);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_swapchain_framebuffers(void)
{
    context.swapchain.framebuffers = (VkFramebuffer*)malloc(context.swapchain.image_count * sizeof(VkFramebuffer));
    for (uint32_t i = 0; i < context.swapchain.image_count; i++)
    {
        VkFramebufferCreateInfo framebuffer_create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = &context.swapchain.image_views[i];
        framebuffer_create_info.renderPass = context.render_pass;
        framebuffer_create_info.width = context.swapchain.extent.width;
        framebuffer_create_info.height = context.swapchain.extent.height;
        framebuffer_create_info.layers = 1;

        VkResult result = vkCreateFramebuffer(context.device.handle, &framebuffer_create_info, NULL, &context.swapchain.framebuffers[i]);
        assert(result == VK_SUCCESS);
    }
}

void mg_vulkan_create_command_pool(void)
{
    VkCommandPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = context.physical_device.graphics_family;

    VkResult result = vkCreateCommandPool(context.device.handle, &pool_info, NULL, &context.command_pool);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_sync_objects(void)
{
    VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateSemaphore(context.device.handle, &semaphoreInfo, NULL, &context.sync_objects.image_available_semaphore);
    assert(result == VK_SUCCESS);

    result = vkCreateSemaphore(context.device.handle, &semaphoreInfo, NULL, &context.sync_objects.image_rendered_semaphore);
    assert(result == VK_SUCCESS);

    result = vkCreateFence(context.device.handle, &fenceInfo, NULL, &context.sync_objects.fence);
    assert(result == VK_SUCCESS);
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

void mg_vulkan_recreate_swapchain(void)
{
    vkDeviceWaitIdle(context.device.handle);
    
    mg_vulkan_cleanup_swapchain();

    mg_vulkan_create_swapchain(context.framebuffer_width, context.framebuffer_height);
    mg_vulkan_create_swapchain_framebuffers();
}

/*void mg_vulkan_create_descriptor_set_layout(void)
{
    VkDescriptorSetLayoutBinding ubo_layout_binding = { 0 };
    ubo_layout_binding.binding = 0;
    ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_layout_binding.descriptorCount = 1;
    ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    ubo_layout_binding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutBinding sampler_layout_binding = { 0 };
    sampler_layout_binding.binding = 0;
    sampler_layout_binding.descriptorCount = 1;
    sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sampler_layout_binding.pImmutableSamplers = NULL;
    sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    
    VkDescriptorSetLayoutCreateInfo layout_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    layout_info.bindingCount = 1;
    layout_info.pBindings = &ubo_layout_binding;

    VkResult result = vkCreateDescriptorSetLayout(context.device.handle, &layout_info, NULL, &context.descripto_set_layouts.ubo);
    assert(result == VK_SUCCESS);

    layout_info.bindingCount = 1;
    layout_info.pBindings = &sampler_layout_binding;

    result = vkCreateDescriptorSetLayout(context.device.handle, &layout_info, NULL, &context.descripto_set_layouts.sampler);
    assert(result == VK_SUCCESS);
}*/

void mg_vulkan_create_descriptor_pool(void)
{
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1024},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4096},
    };

    VkDescriptorPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    pool_info.poolSizeCount = 2;
    pool_info.pPoolSizes = pool_sizes;
    pool_info.maxSets = 1024;

    VkResult result = vkCreateDescriptorPool(context.device.handle, &pool_info, NULL, &context.descriptor_pool);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_renderer_initialize(mg_platform_t *platform)
{
    volkInitialize();
    mg_vulkan_create_instance();

    volkLoadInstance(context.instance);

    mg_vulkan_create_surface(platform);

    mg_vulkan_get_physical_device();
    mg_vulkan_create_device();

    volkLoadDevice(context.device.handle);

    mg_vulkan_create_command_pool();
    context.command_buffer = mg_vulkan_create_command_buffer();

    mg_vulkan_create_sync_objects();

    mg_platform_get_window_size(platform, &context.framebuffer_width, &context.framebuffer_height);
    
    mg_vulkan_create_swapchain(context.framebuffer_width, context.framebuffer_height);
    mg_vulkan_create_render_pass();
    mg_vulkan_create_swapchain_framebuffers();
    mg_vulkan_create_descriptor_pool();
}

void mg_vulkan_renderer_shutdown(void)
{
    vkDeviceWaitIdle(context.device.handle);

    vkDestroyDescriptorPool(context.device.handle, context.descriptor_pool, NULL);

    mg_vulkan_cleanup_swapchain();

    vkDestroyRenderPass(context.device.handle, context.render_pass, NULL);

    vkDestroySemaphore(context.device.handle, context.sync_objects.image_available_semaphore, NULL);
    vkDestroySemaphore(context.device.handle, context.sync_objects.image_rendered_semaphore, NULL);
    vkDestroyFence(context.device.handle, context.sync_objects.fence, NULL);

    vkFreeCommandBuffers(context.device.handle, context.command_pool, 1, &context.command_buffer);
    vkDestroyCommandPool(context.device.handle, context.command_pool, NULL);
    vkDestroyDevice(context.device.handle, NULL);
    vkDestroySurfaceKHR(context.instance, context.surface, NULL);
    vkDestroyInstance(context.instance, NULL);
}

void mg_vulkan_renderer_resize(int32_t width, int32_t height)
{
    context.framebuffer_width = width;
    context.framebuffer_height = height;
    context.framebuffer_resized = true;
}

MG_API void mg_vulkan_renderer_begin_frame(void)
{
    vkWaitForFences(context.device.handle, 1, &context.sync_objects.fence, VK_TRUE, UINT64_MAX);
    
    VkResult result = vkAcquireNextImageKHR(context.device.handle, context.swapchain.handle, UINT64_MAX, context.sync_objects.image_available_semaphore, VK_NULL_HANDLE, &context.image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) 
    {
        mg_vulkan_recreate_swapchain();
        return;
    }

    vkResetFences(context.device.handle, 1, &context.sync_objects.fence);

    VkFenceCreateInfo fence_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkResetCommandBuffer(context.command_buffer, 0);
    
    mg_vulkan_begin_command_buffer(context.command_buffer);

    VkRenderPassBeginInfo render_pass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    render_pass_info.renderPass = context.render_pass;
    render_pass_info.framebuffer = context.swapchain.framebuffers[context.image_index];
    
    render_pass_info.renderArea.offset = (VkOffset2D){0, 0};
    render_pass_info.renderArea.extent = context.swapchain.extent;

    VkClearValue clearColor = {{{0.1f, 0.1f, 0.1f, 1.0f}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(context.command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    mg_vulkan_command_buffer_set_viewport(context.command_buffer, context.swapchain.extent.width, context.swapchain.extent.height);
}

MG_API void mg_vulkan_renderer_end_frame(void)
{
    mg_vulkan_end_command_buffer(context.command_buffer);
    mg_vulkan_submit_command_buffer(context.command_buffer);

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};

    VkSemaphore wait_semaphores[] = {context.sync_objects.image_available_semaphore};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &context.command_buffer;

    VkSemaphore signal_semaphores[] = {context.sync_objects.image_rendered_semaphore};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;
    
    vkQueueSubmit(context.device.graphics_queue, 1, &submit_info, context.sync_objects.fence);
}

MG_API void mg_vulkan_renderer_present(void)
{
    VkSemaphore signal_semaphores[] = {context.sync_objects.image_rendered_semaphore};

    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = {context.swapchain.handle};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &context.image_index;

    VkResult result = vkQueuePresentKHR(context.device.graphics_queue, &present_info);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || context.framebuffer_resized)
    {
        mg_vulkan_recreate_swapchain();
        context.framebuffer_resized = false;
    }
}

void mg_vulkan_renderer_wait(void)
{
    vkDeviceWaitIdle(context.device.handle);
}

void mg_vulkan_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    vkCmdDraw(context.command_buffer, vertex_count, 1, first_vertex, 0);
}

void mg_vulkan_renderer_draw_indexed(uint32_t vertex_index, uint32_t first_index)
{
    vkCmdDrawIndexed(context.command_buffer, vertex_index, 1, first_index, 0, 0);
}