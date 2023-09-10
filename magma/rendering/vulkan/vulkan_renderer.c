#include "vulkan_renderer.h"
#include "vulkan_swapchain.h"
#include "vulkan_command_buffer.h"
#include "vulkan_render_pass.h"
#include "vulkan_buffer.h"
#include "vulkan_image.h"

#include "platform/vulkan_platform.h"

#include <stdlib.h>
#include <assert.h>

mg_vulkan_context_t vulkan_context;

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

    VkResult result = vkCreateInstance(&create_info, NULL, &vulkan_context.instance);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_get_physical_device(void)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(vulkan_context.instance, &device_count, NULL);
    assert(device_count != 0);

    VkPhysicalDevice *devices = (VkPhysicalDevice*)malloc(device_count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(vulkan_context.instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
        vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            deviceFeatures.geometryShader)
        {
            vulkan_context.physical_device.handle = devices[i];
            vulkan_context.physical_device.properties = deviceProperties;
            vulkan_context.physical_device.features = deviceFeatures;
            break;
        }
    }

    free(devices);

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_context.physical_device.handle, &queue_family_count, NULL);

    VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties*)malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_context.physical_device.handle, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++)
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            vulkan_context.physical_device.graphics_family = i;
    
    free(queue_families);
}

void mg_vulkan_create_device(void)
{
    VkDeviceQueueCreateInfo queueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queueCreateInfo.queueFamilyIndex = vulkan_context.physical_device.graphics_family;
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

    VkResult result = vkCreateDevice(vulkan_context.physical_device.handle, &create_info, NULL, &vulkan_context.device.handle);
    assert(result == VK_SUCCESS);

    vkGetDeviceQueue(vulkan_context.device.handle, vulkan_context.physical_device.graphics_family, 0, &vulkan_context.device.graphics_queue);
}

void mg_vulkan_create_command_pool(void)
{
    VkCommandPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = vulkan_context.physical_device.graphics_family;

    VkResult result = vkCreateCommandPool(vulkan_context.device.handle, &pool_info, NULL, &vulkan_context.command_pool);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_sync_objects(void)
{
    VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateSemaphore(vulkan_context.device.handle, &semaphoreInfo, NULL, &vulkan_context.sync_objects.image_available_semaphore);
    assert(result == VK_SUCCESS);

    result = vkCreateSemaphore(vulkan_context.device.handle, &semaphoreInfo, NULL, &vulkan_context.sync_objects.image_rendered_semaphore);
    assert(result == VK_SUCCESS);

    result = vkCreateFence(vulkan_context.device.handle, &fenceInfo, NULL, &vulkan_context.sync_objects.fence);
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

    VkResult result = vkCreateDescriptorPool(vulkan_context.device.handle, &pool_info, NULL, &vulkan_context.descriptor_pool);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_renderer_initialize(mg_renderer_init_info_t *init_info)
{
    volkInitialize();
    mg_vulkan_create_instance();

    volkLoadInstance(vulkan_context.instance);

    mg_vulkan_create_surface(init_info->platform);

    mg_vulkan_get_physical_device();
    mg_vulkan_create_device();

    volkLoadDevice(vulkan_context.device.handle);

    mg_vulkan_create_command_pool();
    vulkan_context.command_buffer = mg_vulkan_create_command_buffer();

    mg_vulkan_create_sync_objects();
    
    mg_vulkan_create_swapchain(init_info->swapchain_config_info);
    
    mg_vulkan_create_descriptor_pool();
}

void mg_vulkan_renderer_shutdown(void)
{
    vkDeviceWaitIdle(vulkan_context.device.handle);

    vkDestroyDescriptorPool(vulkan_context.device.handle, vulkan_context.descriptor_pool, NULL);

    mg_vulkan_cleanup_swapchain();

    vkDestroySemaphore(vulkan_context.device.handle, vulkan_context.sync_objects.image_available_semaphore, NULL);
    vkDestroySemaphore(vulkan_context.device.handle, vulkan_context.sync_objects.image_rendered_semaphore, NULL);
    vkDestroyFence(vulkan_context.device.handle, vulkan_context.sync_objects.fence, NULL);

    mg_vulkan_free_command_buffer(vulkan_context.command_buffer);
    vkDestroyCommandPool(vulkan_context.device.handle, vulkan_context.command_pool, NULL);
    vkDestroyDevice(vulkan_context.device.handle, NULL);
    vkDestroySurfaceKHR(vulkan_context.instance, vulkan_context.surface, NULL);
    vkDestroyInstance(vulkan_context.instance, NULL);
}

void mg_vulkan_renderer_begin_frame(void)
{
    vkWaitForFences(vulkan_context.device.handle, 1, &vulkan_context.sync_objects.fence, VK_TRUE, UINT64_MAX);

    vkAcquireNextImageKHR(vulkan_context.device.handle, vulkan_context.swapchain.handle, UINT64_MAX, vulkan_context.sync_objects.image_available_semaphore, VK_NULL_HANDLE, &vulkan_context.image_index);

    vkResetFences(vulkan_context.device.handle, 1, &vulkan_context.sync_objects.fence);

    VkFenceCreateInfo fence_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkResetCommandBuffer(vulkan_context.command_buffer, 0);
    
    mg_vulkan_begin_command_buffer(vulkan_context.command_buffer);
}

void mg_vulkan_renderer_end_frame(void)
{
    mg_vulkan_end_command_buffer(vulkan_context.command_buffer);
    mg_vulkan_submit_command_buffer(vulkan_context.command_buffer);

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};

    VkSemaphore wait_semaphores[] = {vulkan_context.sync_objects.image_available_semaphore};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vulkan_context.command_buffer;

    VkSemaphore signal_semaphores[] = {vulkan_context.sync_objects.image_rendered_semaphore};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;
    
    vkQueueSubmit(vulkan_context.device.graphics_queue, 1, &submit_info, vulkan_context.sync_objects.fence);
}

void mg_vulkan_renderer_present(void)
{
    VkSemaphore signal_semaphores[] = {vulkan_context.sync_objects.image_rendered_semaphore};

    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = {vulkan_context.swapchain.handle};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &vulkan_context.image_index;

    vkQueuePresentKHR(vulkan_context.device.graphics_queue, &present_info);

    /*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || context.framebuffer_resized)
    {
        mg_vulkan_recreate_swapchain();
        context.framebuffer_resized = false;
    }*/
}

void mg_vulkan_renderer_wait(void)
{
    vkDeviceWaitIdle(vulkan_context.device.handle);
}

void mg_vulkan_renderer_viewport(uint32_t width, uint32_t height)
{
    mg_vulkan_command_buffer_set_viewport(vulkan_context.command_buffer, width, height);
}

void mg_vulkan_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    vkCmdDraw(vulkan_context.command_buffer, vertex_count, 1, first_vertex, 0);
}

void mg_vulkan_renderer_draw_indexed(uint32_t index_count, uint32_t first_index)
{
    vkCmdDrawIndexed(vulkan_context.command_buffer, index_count, 1, first_index, 0, 0);
}

void mg_vulkan_renderer_push_constants(mg_vulkan_program_t *program, uint32_t size, void *data)
{
    vkCmdPushConstants(vulkan_context.command_buffer, program->pipeline_layout, VK_SHADER_STAGE_ALL, 0, size, data);
}