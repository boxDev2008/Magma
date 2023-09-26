#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

uint32_t mg_vulkan_find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(vulkan_context.physical_device.handle, &mem_properties);

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

    VkResult result = vkCreateBuffer(vulkan_context.device.handle, &buffer_info, NULL, buffer);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(vulkan_context.device.handle, *buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = mg_vulkan_find_memory_type(mem_requirements.memoryTypeBits, properties);
    
    result = vkAllocateMemory(vulkan_context.device.handle, &alloc_info, NULL, memory);
    assert(result == VK_SUCCESS);

    vkBindBufferMemory(vulkan_context.device.handle, *buffer, *memory, 0);
}

void mg_vulkan_copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
    VkCommandBuffer command_buffer = mg_vulkan_begin_single_time_commands();

    VkBufferCopy copy_region = { 0 };
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    mg_vulkan_end_single_time_commands(command_buffer);
}


mg_vulkan_buffer_t *mg_vulkan_create_buffer(size_t size, void *data, VkBufferUsageFlags usage)
{
    mg_vulkan_buffer_t *buffer = (mg_vulkan_buffer_t*)malloc(sizeof(mg_vulkan_buffer_t));

    mg_vulkan_allocate_buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &buffer->staging_buffer, &buffer->staging_memory);

    mg_vulkan_allocate_buffer(size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    &buffer->buffer, &buffer->memory);

    void *_data;

    vkMapMemory(vulkan_context.device.handle, buffer->staging_memory, 0, size, 0, &_data);
    memcpy(_data, data, size);
    vkUnmapMemory(vulkan_context.device.handle, buffer->staging_memory);

    mg_vulkan_copy_buffer(buffer->staging_buffer, buffer->buffer, size);

    return buffer;
}

void mg_vulkan_destroy_buffer(mg_vulkan_buffer_t *buffer)
{
    vkDestroyBuffer(vulkan_context.device.handle, buffer->staging_buffer, NULL);
    vkFreeMemory(vulkan_context.device.handle, buffer->staging_memory, NULL);

    vkDestroyBuffer(vulkan_context.device.handle, buffer->buffer, NULL);
    vkFreeMemory(vulkan_context.device.handle, buffer->memory, NULL);

    free(buffer);
}

mg_vulkan_dynamic_buffer_t *mg_vulkan_create_dynamic_buffer(size_t size, VkBufferUsageFlags usage)
{
    mg_vulkan_dynamic_buffer_t *buffer =
        (mg_vulkan_dynamic_buffer_t*)malloc(sizeof(mg_vulkan_dynamic_buffer_t));

    mg_vulkan_allocate_buffer(size, usage,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &buffer->buffer, &buffer->memory);

    vkMapMemory(vulkan_context.device.handle, buffer->memory, 0, size, 0, &buffer->data);

    return buffer;
}

void mg_vulkan_destroy_dynamic_buffer(mg_vulkan_dynamic_buffer_t *buffer)
{
    vkDestroyBuffer(vulkan_context.device.handle, buffer->buffer, NULL);
    vkFreeMemory(vulkan_context.device.handle, buffer->memory, NULL);

    free(buffer);
}

mg_vulkan_vertex_buffer_t *mg_vulkan_create_vertex_buffer(size_t size, void *data)
{
    return mg_vulkan_create_buffer(size, data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void mg_vulkan_destroy_vertex_buffer(mg_vulkan_vertex_buffer_t *buffer)
{
    mg_vulkan_destroy_buffer(buffer);
}

mg_vulkan_index_buffer_t *mg_vulkan_create_index_buffer (size_t size, void *data)
{
    return mg_vulkan_create_buffer(size, data, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void mg_vulkan_destroy_index_buffer(mg_vulkan_index_buffer_t *buffer)
{
    mg_vulkan_destroy_buffer(buffer);
}

mg_vulkan_dynamic_vertex_buffer_t *mg_vulkan_create_dynamic_vertex_buffer(size_t size)
{
    return mg_vulkan_create_dynamic_buffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void mg_vulkan_destroy_dynamic_vertex_buffer(mg_vulkan_dynamic_vertex_buffer_t *buffer)
{
    mg_vulkan_destroy_dynamic_buffer(buffer);
}

void mg_vulkan_update_dynamic_vertex_buffer(mg_vulkan_dynamic_vertex_buffer_t *buffer, size_t size, void *data)
{
    memcpy(buffer->data, data, size);
}

mg_vulkan_dynamic_index_buffer_t *mg_vulkan_create_dynamic_index_buffer(size_t size)
{
    return mg_vulkan_create_dynamic_buffer(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void mg_vulkan_destroy_dynamic_index_buffer(mg_vulkan_dynamic_index_buffer_t *buffer)
{
    mg_vulkan_destroy_dynamic_buffer(buffer);
}

void mg_vulkan_update_dynamic_index_buffer(mg_vulkan_dynamic_index_buffer_t *buffer, size_t size, void *data)
{
    memcpy(buffer->data, data, size);
}

mg_vulkan_uniform_buffer_t *mg_vulkan_create_uniform_buffer(size_t size)
{
    mg_vulkan_uniform_buffer_t *buffer =
        (mg_vulkan_uniform_buffer_t*)malloc(sizeof(mg_vulkan_uniform_buffer_t));

    mg_vulkan_allocate_buffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &buffer->buffer, &buffer->memory);

    vkMapMemory(vulkan_context.device.handle, buffer->memory, 0, size, 0, &buffer->data);

    return buffer;
}

void mg_vulkan_destroy_uniform_buffer(mg_vulkan_uniform_buffer_t *buffer)
{
    vkDestroyBuffer(vulkan_context.device.handle, buffer->buffer, NULL);
    vkFreeMemory(vulkan_context.device.handle, buffer->memory, NULL);

    free(buffer);
}

void mg_vulkan_update_uniform_buffer(mg_vulkan_uniform_buffer_t *buffer, size_t size, void *data)
{
    memcpy(buffer->data, data, size);
}

void mg_vulkan_bind_vertex_buffer(mg_vulkan_vertex_buffer_t *buffer)
{
    VkBuffer vertex_buffers[] = {buffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(vulkan_context.command_buffer, 0, 1, vertex_buffers, offsets);
}

void mg_vulkan_bind_dynamic_vertex_buffer(mg_vulkan_dynamic_vertex_buffer_t *buffer)
{
    VkBuffer vertex_buffers[] = {buffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(vulkan_context.command_buffer, 0, 1, vertex_buffers, offsets);
}

void mg_vulkan_bind_index_buffer(mg_vulkan_index_buffer_t *buffer, mg_index_type_t index_type)
{
    vkCmdBindIndexBuffer(vulkan_context.command_buffer, buffer->buffer, 0, index_type);
}

void mg_vulkan_bind_dynamic_index_buffer(mg_vulkan_dynamic_index_buffer_t *buffer, mg_index_type_t index_type)
{
    vkCmdBindIndexBuffer(vulkan_context.command_buffer, buffer->buffer, 0, index_type);
}