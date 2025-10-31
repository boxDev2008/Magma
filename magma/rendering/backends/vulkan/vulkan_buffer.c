#include "vulkan_buffer.h"
#include "vulkan_resources.h"
#include "vulkan_command_buffer.h"

#include <stdlib.h>
#include <assert.h>

uint32_t mg_vulkan_find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(vk_ctx.physical_device.handle, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    return 0;
}

void mg_vulkan_allocate_buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *memory)
{
    VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(vk_ctx.device.handle, &buffer_info, NULL, buffer);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(vk_ctx.device.handle, *buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = mg_vulkan_find_memory_type(mem_requirements.memoryTypeBits, properties);
    
    result = vkAllocateMemory(vk_ctx.device.handle, &alloc_info, NULL, memory);
    assert(result == VK_SUCCESS);

    vkBindBufferMemory(vk_ctx.device.handle, *buffer, *memory, 0);
}

void mg_vulkan_copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
    VkCommandBuffer command_buffer = mg_vulkan_begin_single_time_commands();

    VkBufferCopy copy_region = { 0 };
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    mg_vulkan_end_single_time_commands(command_buffer);
}

mg_vulkan_buffer *mg_vulkan_create_buffer(size_t size, void *data, VkBufferUsageFlags usage)
{
    mg_vulkan_buffer *buffer = (mg_vulkan_buffer*)malloc(sizeof(mg_vulkan_buffer));

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    mg_vulkan_allocate_buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &staging_buffer, &staging_memory);

    mg_vulkan_allocate_buffer(size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    &buffer->buffer, &buffer->memory);

    void *_data;

    vkMapMemory(vk_ctx.device.handle, staging_memory, 0, size, 0, &_data);
    memcpy(_data, data, size);
    vkUnmapMemory(vk_ctx.device.handle, staging_memory);

    mg_vulkan_copy_buffer(staging_buffer, buffer->buffer, size);

    vkDestroyBuffer(vk_ctx.device.handle, staging_buffer, NULL);
    vkFreeMemory(vk_ctx.device.handle, staging_memory, NULL);

    return buffer;
}

void mg_vulkan_destroy_buffer(mg_vulkan_buffer *buffer)
{
    mg_vulkan_free_resource((mg_vulkan_resource){.type = MG_VULKAN_RESOURCE_TYPE_BUFFER, .buffer = buffer});
}

mg_vulkan_dynamic_buffer *mg_vulkan_create_dynamic_buffer(size_t size, VkBufferUsageFlags usage)
{
    mg_vulkan_dynamic_buffer *buffer =
        (mg_vulkan_dynamic_buffer*)malloc(sizeof(mg_vulkan_dynamic_buffer));

    mg_vulkan_allocate_buffer(size, usage,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &buffer->buffer, &buffer->memory);

    vkMapMemory(vk_ctx.device.handle, buffer->memory, 0, size, 0, &buffer->data);

    return buffer;
}

void mg_vulkan_destroy_dynamic_buffer(mg_vulkan_dynamic_buffer *buffer)
{
    mg_vulkan_free_resource((mg_vulkan_resource){.type = MG_VULKAN_RESOURCE_TYPE_DYNAMIC_BUFFER, .dynamic_buffer = buffer});
}

mg_vulkan_vertex_buffer *mg_vulkan_create_vertex_buffer(size_t size, void *data)
{
    return mg_vulkan_create_buffer(size, data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void mg_vulkan_destroy_vertex_buffer(mg_vulkan_vertex_buffer *buffer)
{
    mg_vulkan_destroy_buffer(buffer);
}

mg_vulkan_index_buffer *mg_vulkan_create_index_buffer (size_t size, void *data)
{
    return mg_vulkan_create_buffer(size, data, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void mg_vulkan_destroy_index_buffer(mg_vulkan_index_buffer *buffer)
{
    mg_vulkan_destroy_buffer(buffer);
}

mg_vulkan_dynamic_vertex_buffer *mg_vulkan_create_dynamic_vertex_buffer(size_t size)
{
    return mg_vulkan_create_dynamic_buffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void mg_vulkan_destroy_dynamic_vertex_buffer(mg_vulkan_dynamic_vertex_buffer *buffer)
{
    mg_vulkan_destroy_dynamic_buffer(buffer);
}

void mg_vulkan_update_dynamic_vertex_buffer(mg_vulkan_dynamic_vertex_buffer *buffer, size_t size, void *data)
{
    memcpy(buffer->data, data, size);
}

mg_vulkan_dynamic_index_buffer *mg_vulkan_create_dynamic_index_buffer(size_t size)
{
    return mg_vulkan_create_dynamic_buffer(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void mg_vulkan_destroy_dynamic_index_buffer(mg_vulkan_dynamic_index_buffer *buffer)
{
    mg_vulkan_destroy_dynamic_buffer(buffer);
}

void mg_vulkan_update_dynamic_index_buffer(mg_vulkan_dynamic_index_buffer *buffer, size_t size, void *data)
{
    memcpy(buffer->data, data, size);
}

void mg_vulkan_bind_vertex_buffer(mg_vulkan_vertex_buffer *buffer)
{
    VkBuffer vertex_buffers[] = {buffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(vk_ctx.command_buffer, 0, 1, vertex_buffers, offsets);
}

void mg_vulkan_bind_dynamic_vertex_buffer(mg_vulkan_dynamic_vertex_buffer *buffer)
{
    VkBuffer vertex_buffers[] = {buffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(vk_ctx.command_buffer, 0, 1, vertex_buffers, offsets);
}

void mg_vulkan_bind_index_buffer(mg_vulkan_index_buffer *buffer, mg_index_type index_type)
{
    vkCmdBindIndexBuffer(vk_ctx.command_buffer, buffer->buffer, 0, index_type);
}

void mg_vulkan_bind_dynamic_index_buffer(mg_vulkan_dynamic_index_buffer *buffer, mg_index_type index_type)
{
    vkCmdBindIndexBuffer(vk_ctx.command_buffer, buffer->buffer, 0, index_type);
}