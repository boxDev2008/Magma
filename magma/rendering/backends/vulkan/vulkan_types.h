#pragma once

#include "base.h"

#include "../../lowl_renderer.h"
#include <vulkan/vulkan.h>

typedef struct mg_vulkan_sampler
{
    VkSamplerAddressMode address_mode;
    VkFilter filter;
    VkSampler sampler;
}
mg_vulkan_sampler;

typedef struct mg_vulkan_image
{
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
}
mg_vulkan_image;

typedef struct mg_vulkan_buffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;
}
mg_vulkan_buffer,
mg_vulkan_vertex_buffer, mg_vulkan_index_buffer;

typedef struct mg_vulkan_dynamic_buffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;

    void *data;
}
mg_vulkan_dynamic_buffer,
mg_vulkan_dynamic_vertex_buffer, mg_vulkan_dynamic_index_buffer;

typedef struct mg_vulkan_uniform_buffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;

    VkDescriptorSet descriptor_set;

    void *data;
}
mg_vulkan_uniform_buffer;

typedef struct mg_vulkan_pipeline
{
	VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
}
mg_vulkan_pipeline;

typedef struct mg_vulkan_context
{
    VkInstance instance;
    VkSurfaceKHR surface;

    struct
    {
        VkPhysicalDevice handle;
        uint32_t graphics_family;
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
    }
    physical_device;

    struct
    {
        VkDevice handle;
        VkQueue graphics_queue;
    }
    device;

    struct
    {
        VkSwapchainKHR handle;

        VkImage *images;
        VkImageView *image_views;
        VkFramebuffer *framebuffers;

        VkImage depth_image;
        VkDeviceMemory depth_image_memory;
        VkImageView depth_image_view;

        uint32_t image_count;
    }
    swapchain;

    struct
    {
        VkFence fence;
        VkSemaphore image_available_semaphore;
        VkSemaphore image_rendered_semaphore;
    }
    sync_objects;

    VkRenderPass render_pass;

    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkDescriptorPool descriptor_pool;

    struct
    {
        VkDescriptorSetLayout uniform_buffer_layout;
        VkDescriptorSetLayout image_sampler_layout;
    }
    layouts;

    struct
    {
        mg_vulkan_pipeline *pipeline;
    }
    binds;

    int32_t image_index;
}
mg_vulkan_context;