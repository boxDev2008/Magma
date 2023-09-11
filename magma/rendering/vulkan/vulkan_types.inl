#pragma once

#include "base.h"
#include "../llapi_renderer.h"

#include <volk.h>

typedef struct mg_vulkan_sampler mg_vulkan_sampler_t;
struct mg_vulkan_sampler
{
    VkSamplerAddressMode address_mode;
    VkFilter filter;
    VkSampler sampler;
};

typedef struct mg_vulkan_texture_image mg_vulkan_texture_image_t;
struct mg_vulkan_texture_image
{
    VkImage image;
    VkDeviceMemory memory;
};

typedef struct mg_vulkan_buffer mg_vulkan_buffer_t;
struct mg_vulkan_buffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    void *data;

    mg_buffer_update_frequency_t frequency;
    bool mapped_at_creation;
};

typedef struct mg_vulkan_pipeline mg_vulkan_pipeline_t;
struct mg_vulkan_pipeline
{
	VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
};

typedef struct mg_vulkan_context mg_vulkan_context_t;
struct mg_vulkan_context
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

    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkDescriptorPool descriptor_pool;

    int32_t image_index;
};