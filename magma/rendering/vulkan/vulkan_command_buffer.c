#include "vulkan_command_buffer.h"

#include "vulkan_renderer.h"

#include <assert.h>

VkCommandBuffer mg_vulkan_create_command_buffer(void)
{
    VkCommandBufferAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc_info.commandPool = vulkan_context.command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;

    VkResult result = vkAllocateCommandBuffers(vulkan_context.device.handle, &alloc_info, &command_buffer);
    assert(result == VK_SUCCESS);

    return command_buffer;
}

void mg_vulkan_submit_command_buffer(VkCommandBuffer buffer)
{
    VkQueue submit_queue = vulkan_context.device.graphics_queue;

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffer;

    vkQueueSubmit(submit_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(submit_queue);
}

VkCommandBuffer mg_vulkan_begin_single_time_commands(void)
{
    VkCommandBufferAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = vulkan_context.command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(vulkan_context.device.handle, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);

    return command_buffer;
}

void mg_vulkan_end_single_time_commands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(vulkan_context.device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(vulkan_context.device.graphics_queue);

    vkFreeCommandBuffers(vulkan_context.device.handle, vulkan_context.command_pool, 1, &commandBuffer);
}

void mg_vulkan_begin_command_buffer(VkCommandBuffer buffer)
{
    VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkResult result = vkBeginCommandBuffer(buffer, &begin_info);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_end_command_buffer(VkCommandBuffer buffer)
{
    VkResult result = vkEndCommandBuffer(buffer);
    assert(result == VK_SUCCESS);
}

void mg_vulkan_free_command_buffer(VkCommandBuffer buffer)
{
    vkFreeCommandBuffers(vulkan_context.device.handle, vulkan_context.command_pool, 1, &buffer);
}

void mg_vulkan_command_buffer_set_viewport(VkCommandBuffer buffer, uint32_t width, uint32_t height)
{
    VkViewport viewport = { 0 };
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = { 0 };
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = width;
    scissor.extent.height = height;

    vkCmdSetViewport(buffer, 0, 1, &viewport);
    vkCmdSetScissor(buffer, 0, 1, &scissor);
}