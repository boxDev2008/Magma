#pragma once

#include "base.h"

#include <volk.h>

typedef struct mg_vulkan_sampler mg_vulkan_sampler_t;
struct mg_vulkan_sampler
{
    VkSamplerAddressMode address_mode;
    VkFilter filter;
    VkSampler sampler;
};

typedef struct mg_vulkan_image mg_vulkan_image_t;
struct mg_vulkan_image
{
    VkImage image;
    VkImageView view;

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

    bool mapped_at_creation;
};

typedef struct mg_vulkan_program mg_vulkan_program_t;
struct mg_vulkan_program
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
        VkPhysicalDeviceProperties2 handle_props;
        VkPhysicalDeviceFeatures2 features;
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

        VkFormat image_format;
        VkExtent2D extent;

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

    int32_t framebuffer_width;
    int32_t framebuffer_height;
    bool framebuffer_resized;

    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkDescriptorPool descriptor_pool;
    VkRenderPass render_pass;

    int32_t image_index;
};