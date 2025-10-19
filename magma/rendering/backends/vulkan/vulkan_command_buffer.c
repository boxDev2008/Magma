#include "vulkan_command_buffer.h"
#include "vulkan_renderer.h"

#include <assert.h>

VkCommandBuffer mg_vulkan_create_command_buffer(void)
{
    VkCommandBufferAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc_info.commandPool = vk_ctx.command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;

    VkResult result = vkAllocateCommandBuffers(vk_ctx.device.handle, &alloc_info, &command_buffer);
    assert(result == VK_SUCCESS);

    return command_buffer;
}

VkCommandBuffer mg_vulkan_begin_single_time_commands(void)
{
    VkCommandBufferAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = vk_ctx.command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(vk_ctx.device.handle, &alloc_info, &command_buffer);

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

    vkQueueSubmit(vk_ctx.device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(vk_ctx.device.graphics_queue);

    vkFreeCommandBuffers(vk_ctx.device.handle, vk_ctx.command_pool, 1, &commandBuffer);
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
    vkFreeCommandBuffers(vk_ctx.device.handle, vk_ctx.command_pool, 1, &buffer);
}

void mg_vulkan_command_buffer_set_viewport(VkCommandBuffer buffer, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    VkViewport viewport = { 0 };
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.x = (float)x;
    viewport.y = (float)y;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = { 0 };
    scissor.offset.x = x;
    scissor.offset.y = y;
    scissor.extent.width = width;
    scissor.extent.height = height;

    vkCmdSetViewport(buffer, 0, 1, &viewport);
    vkCmdSetScissor(buffer, 0, 1, &scissor);
}

void mg_vulkan_command_buffer_set_scissor(VkCommandBuffer buffer, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    VkRect2D scissor = { 0 };
    scissor.offset.x = x;
    scissor.offset.y = y;
    scissor.extent.width = width;
    scissor.extent.height = height;
    vkCmdSetScissor(buffer, 0, 1, &scissor);
}