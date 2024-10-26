#include "vulkan_renderer.h"
#include "vulkan_swapchain.h"
#include "vulkan_command_buffer.h"
#include "vulkan_render_pass.h"
#include "vulkan_buffer.h"
#include "vulkan_image.h"

#include "platform/vulkan_platform.h"

#include <stdlib.h>
#include <assert.h>

mg_vulkan_context vk_context = { 0 };

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
        MG_VULKAN_SURFACE_EXTENSION_NAME
    };

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 2;
    create_info.ppEnabledExtensionNames = instance_extensions;

    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&create_info, NULL, &vk_context.instance);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_get_physical_device(void)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(vk_context.instance, &device_count, NULL);
    assert(device_count != 0);

    VkPhysicalDevice *devices = (VkPhysicalDevice*)malloc(device_count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(vk_context.instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++)
    {
        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceProperties(devices[i], &device_properties);
        vkGetPhysicalDeviceFeatures(devices[i], &device_features);

        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
            device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            vk_context.physical_device.handle = devices[i];
            vk_context.physical_device.properties = device_properties;
            vk_context.physical_device.features = device_features;
            break;
        }
    }

    free(devices);

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vk_context.physical_device.handle, &queue_family_count, NULL);

    VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties*)malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(vk_context.physical_device.handle, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++)
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            vk_context.physical_device.graphics_family = i;
    
    free(queue_families);
}

void mg_vulkan_create_device(void)
{
    VkDeviceQueueCreateInfo queueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queueCreateInfo.queueFamilyIndex = vk_context.physical_device.graphics_family;
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

    VkResult result = vkCreateDevice(vk_context.physical_device.handle, &create_info, NULL, &vk_context.device.handle);
    assert(result == VK_SUCCESS);

    vkGetDeviceQueue(vk_context.device.handle, vk_context.physical_device.graphics_family, 0, &vk_context.device.graphics_queue);
}

void mg_vulkan_create_command_pool(void)
{
    VkCommandPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = vk_context.physical_device.graphics_family;

    VkResult result = vkCreateCommandPool(vk_context.device.handle, &pool_info, NULL, &vk_context.command_pool);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_sync_objects(void)
{
    VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateSemaphore(vk_context.device.handle, &semaphoreInfo, NULL, &vk_context.sync_objects.image_available_semaphore);
    assert(result == VK_SUCCESS);

    result = vkCreateSemaphore(vk_context.device.handle, &semaphoreInfo, NULL, &vk_context.sync_objects.image_rendered_semaphore);
    assert(result == VK_SUCCESS);

    result = vkCreateFence(vk_context.device.handle, &fenceInfo, NULL, &vk_context.sync_objects.fence);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_descriptor_pool(void)
{
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MG_CONFIG_MAX_UNIFORM_BUFFERS},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MG_CONFIG_MAX_TEXTURES},
    };

    VkDescriptorPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.poolSizeCount = 2;
    pool_info.pPoolSizes = pool_sizes;
    pool_info.maxSets = MG_CONFIG_MAX_UNIFORM_BUFFERS + MG_CONFIG_MAX_TEXTURES;

    VkResult result = vkCreateDescriptorPool(vk_context.device.handle, &pool_info, NULL, &vk_context.descriptor_pool);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_descriptor_set_layouts(void)
{
    VkDescriptorSetLayoutCreateInfo layout_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayoutBinding uniform_buffer_layout_binding = (VkDescriptorSetLayoutBinding) {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_ALL
    };

    layout_info.bindingCount = 1;
    layout_info.pBindings = &uniform_buffer_layout_binding;

    VkResult result = vkCreateDescriptorSetLayout(vk_context.device.handle, &layout_info, NULL, &vk_context.layouts.uniform_buffer_layout);
    assert(result == VK_SUCCESS);

    VkDescriptorSetLayoutBinding image_sampler_layout_binding = (VkDescriptorSetLayoutBinding) {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = MG_CONFIG_MAX_BINDABLE_TEXTURES,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    layout_info.pBindings = &image_sampler_layout_binding;

    result = vkCreateDescriptorSetLayout(vk_context.device.handle, &layout_info, NULL, &vk_context.layouts.image_sampler_layout);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_renderer_initialize(mg_lowl_renderer_init_info *init_info)
{
    mg_vulkan_create_instance();

    mg_vulkan_create_surface(init_info->platform);

    mg_vulkan_get_physical_device();
    mg_vulkan_create_device();

    mg_vulkan_create_command_pool();
    vk_context.command_buffer = mg_vulkan_create_command_buffer();

    mg_vulkan_create_sync_objects();

	mg_render_pass_create_info render_pass_info;
	render_pass_info.color_attachment.format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB;
	render_pass_info.depth_stencil_attachment.format = MG_PIXEL_FORMAT_D32_SFLOAT;
	render_pass_info.has_depth_stencil_attachment = true;

    vk_context.render_pass = mg_vulkan_create_render_pass(&render_pass_info);
    mg_vulkan_create_swapchain(init_info->swapchain_config_info);

    mg_vulkan_create_descriptor_set_layouts();
    mg_vulkan_create_descriptor_pool();
}

void mg_vulkan_renderer_shutdown(void)
{
    vkDeviceWaitIdle(vk_context.device.handle);

    vkDestroyDescriptorPool(vk_context.device.handle, vk_context.descriptor_pool, NULL);

    vkDestroyDescriptorSetLayout(vk_context.device.handle, vk_context.layouts.image_sampler_layout, NULL);
    vkDestroyDescriptorSetLayout(vk_context.device.handle, vk_context.layouts.uniform_buffer_layout, NULL);

    mg_vulkan_cleanup_swapchain();

    mg_vulkan_destroy_render_pass(vk_context.render_pass);

    vkDestroySemaphore(vk_context.device.handle, vk_context.sync_objects.image_available_semaphore, NULL);
    vkDestroySemaphore(vk_context.device.handle, vk_context.sync_objects.image_rendered_semaphore, NULL);
    vkDestroyFence(vk_context.device.handle, vk_context.sync_objects.fence, NULL);

    mg_vulkan_free_command_buffer(vk_context.command_buffer);
    vkDestroyCommandPool(vk_context.device.handle, vk_context.command_pool, NULL);

    vkDestroyDevice(vk_context.device.handle, NULL);
    vkDestroySurfaceKHR(vk_context.instance, vk_context.surface, NULL);
    vkDestroyInstance(vk_context.instance, NULL);
}

void mg_vulkan_renderer_begin_frame(void)
{
    vkWaitForFences(vk_context.device.handle, 1, &vk_context.sync_objects.fence, VK_TRUE, UINT64_MAX);
    vkAcquireNextImageKHR(vk_context.device.handle, vk_context.swapchain.handle, UINT64_MAX, vk_context.sync_objects.image_available_semaphore, VK_NULL_HANDLE, &vk_context.image_index);
   
    vkResetFences(vk_context.device.handle, 1, &vk_context.sync_objects.fence);

    VkFenceCreateInfo fence_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkResetCommandBuffer(vk_context.command_buffer, 0);
    
    mg_vulkan_begin_command_buffer(vk_context.command_buffer);
}

void mg_vulkan_renderer_end_frame(void)
{
    mg_vulkan_end_command_buffer(vk_context.command_buffer);
    mg_vulkan_submit_command_buffer(vk_context.command_buffer);

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};

    VkSemaphore wait_semaphores[] = {vk_context.sync_objects.image_available_semaphore};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vk_context.command_buffer;

    VkSemaphore signal_semaphores[] = {vk_context.sync_objects.image_rendered_semaphore};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;
    
    vkQueueSubmit(vk_context.device.graphics_queue, 1, &submit_info, vk_context.sync_objects.fence);
}

void mg_vulkan_renderer_present_frame(void)
{
    VkSemaphore signal_semaphores[] = {vk_context.sync_objects.image_rendered_semaphore};

    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = {vk_context.swapchain.handle};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &vk_context.image_index;

    vkQueuePresentKHR(vk_context.device.graphics_queue, &present_info);
}

void mg_vulkan_renderer_wait(void)
{
    vkDeviceWaitIdle(vk_context.device.handle);
}

void mg_vulkan_renderer_viewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    mg_vulkan_command_buffer_set_viewport(vk_context.command_buffer, x, y, width, height);
}

void mg_vulkan_renderer_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    mg_vulkan_command_buffer_set_scissor(vk_context.command_buffer, x, y, width, height);
}

/*void mg_vulkan_renderer_bind_descriptors(void)
{
    if (vk_context.update_image_array)
    {
        VkWriteDescriptorSet write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = MG_CONFIG_MAX_BINDABLE_TEXTURES;
        write.pBufferInfo = 0;
        write.pImageInfo = vk_context.image_sampler_infos;
        vk_context.extensions.vkCmdPushDescriptorSetKHR(vk_context.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_context.binds.pipeline->pipeline_layout, 1, 1, &write);
        vk_context.update_image_array = false;
    }
}*/

void mg_vulkan_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    //mg_vulkan_renderer_bind_descriptors();
    vkCmdDraw(vk_context.command_buffer, vertex_count, 1, first_vertex, 0);
}

void mg_vulkan_renderer_draw_indexed(uint32_t index_count, uint32_t first_index)
{
    //mg_vulkan_renderer_bind_descriptors();
    vkCmdDrawIndexed(vk_context.command_buffer, index_count, 1, first_index, 0, 0);
}

void mg_vulkan_renderer_push_constants(uint32_t size, void *data)
{
    vkCmdPushConstants(vk_context.command_buffer, vk_context.binds.pipeline->pipeline_layout, VK_SHADER_STAGE_ALL, 0, size, data);
}