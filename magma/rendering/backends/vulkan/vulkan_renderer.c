#include "vulkan_renderer.h"
#include "vulkan_swapchain.h"
#include "vulkan_command_buffer.h"
#include "vulkan_render_pass.h"
#include "vulkan_buffer.h"
#include "vulkan_image.h"

#include "platform/vulkan_platform.h"

#include <stdlib.h>
#include <assert.h>

#pragma comment (lib, "vulkan-1")

mg_vulkan_context vk_ctx = { 0 };

static inline uint32_t mg_stride_align(uint32_t size, uint32_t alignment)
{
    return (size+(alignment-1)) & ~(alignment-1);
}

/**
 * @brief Creates a Vulkan instance.
 *
 * Initializes Vulkan by configuring application-specific information and determining
 * the required instance extensions based on the operating system. On Windows, it enables
 * the surface and Win32 surface extensions; on Linux, it retrieves the necessary extensions
 * via GLFW. The created instance is stored in the global Vulkan context.
 *
 * An assertion is triggered if instance creation fails.
 */
void mg_vulkan_create_instance(void)
{
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pApplicationName = NULL;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Magma Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;

#if MG_PLATFORM_WINDOWS
    const char *instance_extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };
    create_info.enabledExtensionCount = 2;
#elif MG_PLATFORM_LINUX
	const char **instance_extensions = glfwGetRequiredInstanceExtensions(&create_info.enabledExtensionCount);
#endif
    create_info.ppEnabledExtensionNames = instance_extensions;

    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&create_info, NULL, &vk_ctx.instance);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_get_physical_device(void)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(vk_ctx.instance, &device_count, NULL);
    assert(device_count != 0);

    VkPhysicalDevice *devices = (VkPhysicalDevice*)malloc(device_count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(vk_ctx.instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++)
    {
        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceProperties(devices[i], &device_properties);
        vkGetPhysicalDeviceFeatures(devices[i], &device_features);

        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
            device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            vk_ctx.physical_device.handle = devices[i];
            vk_ctx.physical_device.properties = device_properties;
            vk_ctx.physical_device.features = device_features;
            break;
        }
    }

    free(devices);

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vk_ctx.physical_device.handle, &queue_family_count, NULL);

    VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties*)malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(vk_ctx.physical_device.handle, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++)
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            vk_ctx.physical_device.graphics_family = i;
    
    free(queue_families);
}

void mg_vulkan_create_device(void)
{
    VkDeviceQueueCreateInfo queueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queueCreateInfo.queueFamilyIndex = vk_ctx.physical_device.graphics_family;
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

    VkResult result = vkCreateDevice(vk_ctx.physical_device.handle, &create_info, NULL, &vk_ctx.device.handle);
    assert(result == VK_SUCCESS);

    vkGetDeviceQueue(vk_ctx.device.handle, vk_ctx.physical_device.graphics_family, 0, &vk_ctx.device.graphics_queue);
}

void mg_vulkan_create_command_pool(void)
{
    VkCommandPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = vk_ctx.physical_device.graphics_family;

    VkResult result = vkCreateCommandPool(vk_ctx.device.handle, &pool_info, NULL, &vk_ctx.command_pool);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_sync_objects(void)
{
    VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateSemaphore(vk_ctx.device.handle, &semaphoreInfo, NULL, &vk_ctx.sync_objects.image_available_semaphore);
    assert(result == VK_SUCCESS);

    result = vkCreateSemaphore(vk_ctx.device.handle, &semaphoreInfo, NULL, &vk_ctx.sync_objects.image_rendered_semaphore);
    assert(result == VK_SUCCESS);

    result = vkCreateFence(vk_ctx.device.handle, &fenceInfo, NULL, &vk_ctx.sync_objects.fence);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_descriptor_pool(void)
{
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MG_CONFIG_MAX_IMAGES_ARRAYS},
    };

    VkDescriptorPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.poolSizeCount = 2;
    pool_info.pPoolSizes = pool_sizes;
    pool_info.maxSets = 1 + MG_CONFIG_MAX_IMAGES_ARRAYS;

    VkResult result = vkCreateDescriptorPool(vk_ctx.device.handle, &pool_info, NULL, &vk_ctx.descriptor_pool);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_descriptor_set_layouts(void)
{
    VkDescriptorSetLayoutCreateInfo layout_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayoutBinding scratch_buffer_layout_bindings[MG_CONFIG_MAX_BINDABLE_UNIFORMS];

    for (uint32_t i = 0; i < MG_CONFIG_MAX_BINDABLE_UNIFORMS; i++)
    {
        scratch_buffer_layout_bindings[i] = (VkDescriptorSetLayoutBinding) {
            .binding = i,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL
        };
    }

    layout_info.bindingCount = MG_CONFIG_MAX_BINDABLE_UNIFORMS;
    layout_info.pBindings = scratch_buffer_layout_bindings;

    VkResult result = vkCreateDescriptorSetLayout(vk_ctx.device.handle, &layout_info, NULL, &vk_ctx.layouts.scratch_buffer_layout);
    assert(result == VK_SUCCESS);

    VkDescriptorSetLayoutBinding image_sampler_layout_binding = (VkDescriptorSetLayoutBinding) {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = MG_CONFIG_MAX_BINDABLE_IMAGES,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    layout_info.bindingCount = 1;
    layout_info.pBindings = &image_sampler_layout_binding;

    result = vkCreateDescriptorSetLayout(vk_ctx.device.handle, &layout_info, NULL, &vk_ctx.layouts.image_sampler_layout);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_create_scratch_buffer(void)
{
    mg_vulkan_allocate_buffer(MG_CONFIG_MAX_SCRATCH_BUFFER_SIZE, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &vk_ctx.scratch_buffer.buffer, &vk_ctx.scratch_buffer.memory);
    vkMapMemory(vk_ctx.device.handle, vk_ctx.scratch_buffer.memory, 0, MG_CONFIG_MAX_SCRATCH_BUFFER_SIZE, 0, &vk_ctx.scratch_buffer.data);

    VkDescriptorSetAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    alloc_info.descriptorPool = vk_ctx.descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &vk_ctx.layouts.scratch_buffer_layout;

    VkResult result = vkAllocateDescriptorSets(vk_ctx.device.handle, &alloc_info, &vk_ctx.scratch_buffer.ub_set);
    assert(result == VK_SUCCESS);

    VkWriteDescriptorSet writes[MG_CONFIG_MAX_BINDABLE_UNIFORMS] = { 0 };
    VkDescriptorBufferInfo buffer_infos[MG_CONFIG_MAX_BINDABLE_UNIFORMS] = { 0 };

    for (uint32_t i = 0; i < MG_CONFIG_MAX_BINDABLE_UNIFORMS; i++)
    {
        VkWriteDescriptorSet *write = &writes[i];
        VkDescriptorBufferInfo *buffer_info = &buffer_infos[i];

        write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write->dstSet = vk_ctx.scratch_buffer.ub_set;
        write->dstBinding = i;
        write->dstArrayElement = 0;
    
        write->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        write->descriptorCount = 1;

        buffer_info->buffer = vk_ctx.scratch_buffer.buffer;
        buffer_info->offset = 0;
        buffer_info->range = MG_CONFIG_MAX_UNIFORM_UPDATE_SIZE;
    
        write->pBufferInfo = buffer_info;
    }

    vkUpdateDescriptorSets(vk_ctx.device.handle, MG_CONFIG_MAX_BINDABLE_UNIFORMS, writes, 0, NULL);
}

void mg_vulkan_renderer_initialize(mgfx_init_info *init_info)
{
    mg_vulkan_create_instance();

    mg_vulkan_create_surface(init_info->platform);

    mg_vulkan_get_physical_device();
    mg_vulkan_create_device();

    mg_vulkan_create_command_pool();
    vk_ctx.command_buffer = mg_vulkan_create_command_buffer();

    mg_vulkan_create_sync_objects();

	mg_render_pass_create_info render_pass_info;
	render_pass_info.color_attachment.format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB;
	render_pass_info.depth_stencil_attachment.format = MG_PIXEL_FORMAT_D32_SFLOAT;
	render_pass_info.has_depth_stencil_attachment = true;

    vk_ctx.render_pass = mg_vulkan_create_render_pass(&render_pass_info);
    mg_vulkan_create_swapchain(init_info->swapchain_config_info);

    mg_vulkan_create_descriptor_set_layouts();
    mg_vulkan_create_descriptor_pool();

    mg_vulkan_create_scratch_buffer();
}

void mg_vulkan_renderer_shutdown(void)
{
    vkDeviceWaitIdle(vk_ctx.device.handle);

    vkDestroyDescriptorPool(vk_ctx.device.handle, vk_ctx.descriptor_pool, NULL);

    vkDestroyDescriptorSetLayout(vk_ctx.device.handle, vk_ctx.layouts.image_sampler_layout, NULL);
    vkDestroyDescriptorSetLayout(vk_ctx.device.handle, vk_ctx.layouts.scratch_buffer_layout, NULL);

    vkDestroyBuffer(vk_ctx.device.handle, vk_ctx.scratch_buffer.buffer, NULL);
    vkFreeMemory(vk_ctx.device.handle, vk_ctx.scratch_buffer.memory, NULL);
    vkFreeDescriptorSets(vk_ctx.device.handle, vk_ctx.descriptor_pool, 1, &vk_ctx.scratch_buffer.ub_set);

    mg_vulkan_cleanup_swapchain();

    mg_vulkan_destroy_render_pass(vk_ctx.render_pass);

    vkDestroySemaphore(vk_ctx.device.handle, vk_ctx.sync_objects.image_available_semaphore, NULL);
    vkDestroySemaphore(vk_ctx.device.handle, vk_ctx.sync_objects.image_rendered_semaphore, NULL);
    vkDestroyFence(vk_ctx.device.handle, vk_ctx.sync_objects.fence, NULL);

    mg_vulkan_free_command_buffer(vk_ctx.command_buffer);
    vkDestroyCommandPool(vk_ctx.device.handle, vk_ctx.command_pool, NULL);

    vkDestroyDevice(vk_ctx.device.handle, NULL);
    vkDestroySurfaceKHR(vk_ctx.instance, vk_ctx.surface, NULL);
    vkDestroyInstance(vk_ctx.instance, NULL);
}

void mg_vulkan_renderer_begin(void)
{
    vkWaitForFences(vk_ctx.device.handle, 1, &vk_ctx.sync_objects.fence, VK_TRUE, UINT64_MAX);
    vkAcquireNextImageKHR(vk_ctx.device.handle, vk_ctx.swapchain.handle, UINT64_MAX, vk_ctx.sync_objects.image_available_semaphore, VK_NULL_HANDLE, &vk_ctx.image_index);
   
    vkResetFences(vk_ctx.device.handle, 1, &vk_ctx.sync_objects.fence);

    VkFenceCreateInfo fence_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkResetCommandBuffer(vk_ctx.command_buffer, 0);
    
    mg_vulkan_begin_command_buffer(vk_ctx.command_buffer);

    vk_ctx.scratch_buffer.offset = 0;
}

void mg_vulkan_renderer_end(void)
{
    mg_vulkan_end_command_buffer(vk_ctx.command_buffer);

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vk_ctx.command_buffer;

    VkSemaphore wait_semaphores[] = {vk_ctx.sync_objects.image_available_semaphore};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    VkSemaphore signal_semaphores[] = {vk_ctx.sync_objects.image_rendered_semaphore};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;
    
    vkQueueSubmit(vk_ctx.device.graphics_queue, 1, &submit_info, vk_ctx.sync_objects.fence);

    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = {vk_ctx.swapchain.handle};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &vk_ctx.image_index;

    vkQueuePresentKHR(vk_ctx.device.graphics_queue, &present_info);
}

void mg_vulkan_renderer_wait(void)
{
    vkDeviceWaitIdle(vk_ctx.device.handle);
}

void mg_vulkan_renderer_viewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    mg_vulkan_command_buffer_set_viewport(vk_ctx.command_buffer, x, y, width, height);
}

void mg_vulkan_renderer_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    mg_vulkan_command_buffer_set_scissor(vk_ctx.command_buffer, x, y, width, height);
}

/*void mg_vulkan_renderer_bind_descriptors(void)
{
    if (vk_ctx.update_image_array)
    {
        VkWriteDescriptorSet write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = MG_CONFIG_MAX_BINDABLE_IMAGES;
        write.pBufferInfo = 0;
        write.pImageInfo = vk_ctx.image_sampler_infos;
        vk_ctx.extensions.vkCmdPushDescriptorSetKHR(vk_ctx.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_ctx.binds.pipeline->pipeline_layout, 1, 1, &write);
        vk_ctx.update_image_array = false;
    }
}*/

void mg_vulkan_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    vkCmdDraw(vk_ctx.command_buffer, vertex_count, 1, first_vertex, 0);
}

void mg_vulkan_renderer_draw_indexed(uint32_t index_count, uint32_t first_index, int32_t first_vertex)
{
    vkCmdDrawIndexed(vk_ctx.command_buffer, index_count, 1, first_index, first_vertex, 0);
}

void mg_vulkan_renderer_bind_uniforms(uint32_t binding, size_t size, void *data)
{
    const uint32_t alignment = vk_ctx.physical_device.properties.limits.minUniformBufferOffsetAlignment;
    memcpy(vk_ctx.scratch_buffer.data + vk_ctx.scratch_buffer.offset, data, size);
    vk_ctx.scratch_buffer.bind_offsets[binding] = vk_ctx.scratch_buffer.offset;
    vk_ctx.scratch_buffer.offset = mg_stride_align(vk_ctx.scratch_buffer.offset + (uint32_t)size, alignment);

    vkCmdBindDescriptorSets(
        vk_ctx.command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        vk_ctx.binds.pipeline->pipeline_layout,
        0,
        1,
        &vk_ctx.scratch_buffer.ub_set,
        MG_CONFIG_MAX_BINDABLE_UNIFORMS,
        vk_ctx.scratch_buffer.bind_offsets
    );
}