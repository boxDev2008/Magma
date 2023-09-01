#include "vulkan_command_buffer.h"

#include "vulkan_renderer.h"

#include <assert.h>

VkCommandBuffer mg_vulkan_create_command_buffer(void)
{
    VkCommandBufferAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc_info.commandPool = context.command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;

    VkResult result = vkAllocateCommandBuffers(context.device.handle, &alloc_info, &command_buffer);
    assert(result == VK_SUCCESS);

    return command_buffer;
}

void mg_vulkan_submit_command_buffer(VkCommandBuffer buffer)
{
    VkQueue submit_queue = context.device.graphics_queue;

    VkSubmitInfo submit_info = { 0 };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffer;

    vkQueueSubmit(submit_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(submit_queue);
}

VkCommandBuffer mg_vulkan_begin_single_time_commands(void)
{
    VkCommandBufferAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = context.command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(context.device.handle, &alloc_info, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void mg_vulkan_end_single_time_commands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(context.device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(context.device.graphics_queue);

    vkFreeCommandBuffers(context.device.handle, context.command_pool, 1, &commandBuffer);
}

void mg_vulkan_begin_command_buffer(VkCommandBuffer buffer)
{
    VkCommandBufferBeginInfo begin_info = { 0 };
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
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
    vkFreeCommandBuffers(context.device.handle, context.command_pool, 1, &buffer);
}

void mg_vulkan_command_buffer_set_viewport(VkCommandBuffer buffer, int32_t width, int32_t height)
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
    scissor.extent.width = (uint32_t)width;
    scissor.extent.height = (uint32_t)height;

    vkCmdSetViewport(buffer, 0, 1, &viewport);
    vkCmdSetScissor(buffer, 0, 1, &scissor);
}