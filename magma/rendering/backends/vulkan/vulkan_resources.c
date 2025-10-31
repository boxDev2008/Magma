#include "vulkan_resources.h"
#include "vulkan_renderer.h"

#include <stdio.h>
#include <assert.h>

void mg_vulkan_recycle(void)
{
    while (!mg_is_stack_empty(&vk_ctx.freed_resources))
    {
        mg_vulkan_resource *resource = mg_stack_pop(&vk_ctx.freed_resources);
        switch (resource->type)
        {
        case MG_VULKAN_RESOURCE_TYPE_BUFFER:
            vkDestroyBuffer(vk_ctx.device.handle, resource->buffer->buffer, NULL);
            vkFreeMemory(vk_ctx.device.handle, resource->buffer->memory, NULL);
            free(resource->buffer);
            break;
        case MG_VULKAN_RESOURCE_TYPE_DYNAMIC_BUFFER:
            vkUnmapMemory(vk_ctx.device.handle, resource->dynamic_buffer->memory);
            vkDestroyBuffer(vk_ctx.device.handle, resource->dynamic_buffer->buffer, NULL);
            vkFreeMemory(vk_ctx.device.handle, resource->dynamic_buffer->memory, NULL);
            free(resource->dynamic_buffer);
            break;
        case MG_VULKAN_RESOURCE_TYPE_IMAGE:
            vkDestroyImage(vk_ctx.device.handle, resource->image->image, NULL);
            vkFreeMemory(vk_ctx.device.handle, resource->image->memory, NULL);
            vkDestroyImageView(vk_ctx.device.handle, resource->image->view, NULL);
            free(resource->image);
            break;
        case MG_VULKAN_RESOURCE_TYPE_RENDER_PASS:
            vkDestroyRenderPass(vk_ctx.device.handle, resource->render_pass->render_pass, NULL);
            vkDestroyFramebuffer(vk_ctx.device.handle, resource->render_pass->framebuffer, NULL);
            free(resource->render_pass);
            break;
        case MG_VULKAN_RESOURCE_TYPE_PIPELINE:
            vkDestroyPipeline(vk_ctx.device.handle, resource->pipeline->pipeline, NULL);
            vkDestroyPipelineLayout(vk_ctx.device.handle, resource->pipeline->pipeline_layout, NULL);
            free(resource->pipeline);
            break;
        case MG_VULKAN_RESOURCE_TYPE_SAMPLER:
            vkDestroySampler(vk_ctx.device.handle, resource->sampler, NULL);
            break;
        }
    }
}

void mg_vulkan_free_resource(const mg_vulkan_resource resource)
{
    mg_stack_push(&vk_ctx.freed_resources, &resource);
}