#include "vulkan_renderer.h"
#include "vulkan_swapchain.h"
#include "vulkan_command_buffer.h"
#include "vulkan_render_pass.h"
#include "vulkan_buffer.h"
#include "vulkan_image.h"

#include "platform/vulkan_platform.h"

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
            context.physical_device.properties = deviceProperties;
            context.physical_device.features = deviceFeatures;
            break;
        }
    }

    free(devices);

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

void mg_vulkan_renderer_initialize(mg_renderer_init_info_t *init_info)
{
    volkInitialize();
    mg_vulkan_create_instance();

    volkLoadInstance(context.instance);

    mg_vulkan_create_surface(init_info->platform);

    mg_vulkan_get_physical_device();
    mg_vulkan_create_device();

    volkLoadDevice(context.device.handle);

    mg_vulkan_create_command_pool();
    context.command_buffer = mg_vulkan_create_command_buffer();

    mg_vulkan_create_sync_objects();
    
    mg_vulkan_create_swapchain(init_info->swapchain_config_info);
    
    mg_vulkan_create_descriptor_pool();
}

void mg_vulkan_renderer_shutdown(void)
{
    vkDeviceWaitIdle(context.device.handle);

    vkDestroyDescriptorPool(context.device.handle, context.descriptor_pool, NULL);

    mg_vulkan_cleanup_swapchain();

    vkDestroySemaphore(context.device.handle, context.sync_objects.image_available_semaphore, NULL);
    vkDestroySemaphore(context.device.handle, context.sync_objects.image_rendered_semaphore, NULL);
    vkDestroyFence(context.device.handle, context.sync_objects.fence, NULL);

    mg_vulkan_free_command_buffer(context.command_buffer);
    vkDestroyCommandPool(context.device.handle, context.command_pool, NULL);
    vkDestroyDevice(context.device.handle, NULL);
    vkDestroySurfaceKHR(context.instance, context.surface, NULL);
    vkDestroyInstance(context.instance, NULL);
}

void mg_vulkan_renderer_begin_frame(void)
{
    vkWaitForFences(context.device.handle, 1, &context.sync_objects.fence, VK_TRUE, UINT64_MAX);

    vkAcquireNextImageKHR(context.device.handle, context.swapchain.handle, UINT64_MAX, context.sync_objects.image_available_semaphore, VK_NULL_HANDLE, &context.image_index);

    vkResetFences(context.device.handle, 1, &context.sync_objects.fence);

    VkFenceCreateInfo fence_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkResetCommandBuffer(context.command_buffer, 0);
    
    mg_vulkan_begin_command_buffer(context.command_buffer);
}

void mg_vulkan_renderer_end_frame(void)
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

void mg_vulkan_renderer_present(void)
{
    VkSemaphore signal_semaphores[] = {context.sync_objects.image_rendered_semaphore};

    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = {context.swapchain.handle};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &context.image_index;

    vkQueuePresentKHR(context.device.graphics_queue, &present_info);

    /*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || context.framebuffer_resized)
    {
        mg_vulkan_recreate_swapchain();
        context.framebuffer_resized = false;
    }*/
}

void mg_vulkan_renderer_wait(void)
{
    vkDeviceWaitIdle(context.device.handle);
}

void mg_vulkan_renderer_viewport(uint32_t width, uint32_t height)
{
    mg_vulkan_command_buffer_set_viewport(context.command_buffer, width, height);
}

void mg_vulkan_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    vkCmdDraw(context.command_buffer, vertex_count, 1, first_vertex, 0);
}

void mg_vulkan_renderer_draw_indexed(uint32_t index_count, uint32_t first_index)
{
    vkCmdDrawIndexed(context.command_buffer, index_count, 1, first_index, 0, 0);
}

void mg_vulkan_renderer_push_constants(mg_vulkan_program_t *program, uint32_t size, void *data)
{
    vkCmdPushConstants(context.command_buffer, program->pipeline_layout, VK_SHADER_STAGE_ALL, 0, size, data);
}