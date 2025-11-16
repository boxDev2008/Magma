#pragma once

#include "base.h"
#include "memory/stack.h"

#include "../../renderer.h"

#include <vulkan/vulkan.h>

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
    bool is_cpu;
    void *data;
}
mg_vulkan_buffer;

typedef struct mg_vulkan_pipeline
{
	VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;
    VkPipelineBindPoint bind_point;
}
mg_vulkan_pipeline;

typedef struct mg_vulkan_render_pass
{
    VkRenderPass render_pass;
    VkFramebuffer framebuffer;
}
mg_vulkan_render_pass;

typedef enum mg_vulkan_resource_type
{
    MG_VULKAN_RESOURCE_TYPE_BUFFER,
    MG_VULKAN_RESOURCE_TYPE_IMAGE,
    MG_VULKAN_RESOURCE_TYPE_PIPELINE,
    MG_VULKAN_RESOURCE_TYPE_RENDER_PASS,
    MG_VULKAN_RESOURCE_TYPE_SAMPLER
}
mg_vulkan_resource_type;

typedef struct mg_vulkan_resource
{
    mg_vulkan_resource_type type;
    union
    {
        mg_vulkan_buffer *buffer;
        mg_vulkan_image *image;
        mg_vulkan_pipeline *pipeline;
        mg_vulkan_render_pass *render_pass;
        VkSampler sampler;
    };
}
mg_vulkan_resource;

typedef struct mg_vulkan_descriptor_binding
{
    uint32_t binding;
    union
    {
        struct
        {
            VkImageView image_view;
            VkSampler sampler;
        };
        VkBuffer buffer;
    };
}
mg_vulkan_descriptor_binding;

typedef struct mg_vulkan_descriptor_set_key
{
    mg_vulkan_descriptor_binding bindings[MG_CONFIG_MAX_BINDABLE_IMAGES];
    uint32_t binding_count;
    uint64_t hash;
}
mg_vulkan_descriptor_set_key;

typedef struct mg_vulkan_cached_descriptor_set
{
    mg_vulkan_descriptor_set_key key;
}
mg_vulkan_cached_descriptor_set;

typedef struct mg_vulkan_descriptor_cache
{    
    VkDescriptorSet sets[MG_CONFIG_MAX_DESCRIPTOR_CACHE];
    mg_vulkan_cached_descriptor_set cache[MG_CONFIG_MAX_DESCRIPTOR_CACHE];
    uint32_t cache_size;

    mg_vulkan_descriptor_binding pending_image_bindings[MG_CONFIG_MAX_BINDABLE_IMAGES];
    uint32_t pending_image_binding_count;
}
mg_vulkan_descriptor_cache;

typedef struct mg_vulkan_context
{
    VkInstance instance;
    VkSurfaceKHR surface;

    struct
    {
        VkPhysicalDevice handle;
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        uint32_t queue_family;
    }
    physical_device;

    struct
    {
        VkDevice handle;
        VkQueue graphics_compute_queue;
    }
    device;

    struct
    {
        VkSwapchainKHR handle;
        VkFormat color_format;

        VkImage images[4];
        VkImageView image_views[4];
        VkFramebuffer framebuffers[4];

        VkImage depth_image;
        VkDeviceMemory depth_image_memory;
        VkImageView depth_image_view;

        uint32_t image_count;
        int32_t image_index;
    }
    swapchain;

    struct
    {
        VkFence fence;
        VkSemaphore image_available_semaphore;
        VkSemaphore image_rendered_semaphore;
    }
    sync_objects;

    bool inside_render_pass;
    VkRenderPass default_render_pass;

    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkDescriptorPool descriptor_pool;

    struct
    {
        VkDescriptorSetLayout scratch_buffer_layout;
        VkDescriptorSetLayout image_sampler_layout;
        VkDescriptorSetLayout storage_buffer_layout;
    }
    layouts;

    mg_vulkan_pipeline *current_pipeline;

    struct
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
        uint8_t *data;

        uint32_t bind_offsets[MG_CONFIG_MAX_BINDABLE_UNIFORMS];
        uint32_t offset;

        VkDescriptorSet ub_set;
    }
    scratch_buffer;

    mg_vulkan_descriptor_cache descriptor_cache;
    mg_stack freed_resources;
}
mg_vulkan_context;