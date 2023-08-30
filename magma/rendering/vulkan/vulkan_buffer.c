#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

uint32_t mg_vulkan_find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(context.physical_device.handle, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    return 0;
}

void mg_vulkan_create_staging_buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, mg_vulkan_staging_buffer_t *out_buffer)
{
    VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(context.device.handle, &buffer_info, NULL, &out_buffer->buffer);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(context.device.handle, out_buffer->buffer, &mem_requirements);

    VkMemoryAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocInfo.allocationSize = mem_requirements.size;
    allocInfo.memoryTypeIndex = mg_vulkan_find_memory_type(mem_requirements.memoryTypeBits, properties);
    
    result = vkAllocateMemory(context.device.handle, &allocInfo, NULL, &out_buffer->memory);
    assert(result == VK_SUCCESS);

    vkBindBufferMemory(context.device.handle, out_buffer->buffer, out_buffer->memory, 0);
}

void mg_vulkan_destroy_buffer(mg_vulkan_buffer_t *buffer)
{
    vkDestroyBuffer(context.device.handle, buffer->buffer, NULL);
    vkFreeMemory(context.device.handle, buffer->memory, NULL);
}

void mg_vulkan_copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
    VkCommandBuffer command_buffer = mg_vulkan_begin_single_time_commands();

    VkBufferCopy copy_region = { 0 };
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    mg_vulkan_end_single_time_commands(command_buffer);
}

mg_vulkan_buffer_t *mg_vulkan_create_vertex_buffer(void *vertices, size_t size)
{
    mg_vulkan_buffer_t *buffer = (mg_vulkan_buffer_t*)malloc(sizeof(mg_vulkan_buffer_t));

    mg_vulkan_buffer_t staging_buffer;
    mg_vulkan_create_staging_buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer);

    void* data;
    vkMapMemory(context.device.handle, staging_buffer.memory, 0, size, 0, &data);
        memcpy(data, vertices, size);
    vkUnmapMemory(context.device.handle, staging_buffer.memory);

    mg_vulkan_create_buffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer);

    mg_vulkan_copy_buffer(staging_buffer.buffer, buffer->buffer, size);

    mg_vulkan_destroy_buffer(&staging_buffer);

    return buffer;
}

mg_vulkan_buffer_t *mg_vulkan_create_index_buffer(void *indices, size_t size)
{
    mg_vulkan_buffer_t *buffer = (mg_vulkan_buffer_t*)malloc(sizeof(mg_vulkan_buffer_t));

    mg_vulkan_buffer_t staging_buffer;
    mg_vulkan_create_buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer);

    void* data;
    vkMapMemory(context.device.handle, staging_buffer.memory, 0, size, 0, &data);
        memcpy(data, indices, size);
    vkUnmapMemory(context.device.handle, staging_buffer.memory);

    mg_vulkan_create_buffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer);

    mg_vulkan_copy_buffer(staging_buffer.buffer, buffer->buffer, size);

    mg_vulkan_destroy_buffer(&staging_buffer);

    return buffer;
}

void mg_vulkan_update_buffer(mg_vulkan_buffer_t *buffer, void *data, size_t size)
{

}

mg_vulkan_uniform_buffer_t *mg_vulkan_create_uniform_buffer(size_t size)
{
    mg_vulkan_uniform_buffer_t *buffer = (mg_vulkan_uniform_buffer_t*)malloc(sizeof(mg_vulkan_uniform_buffer_t));

    mg_vulkan_create_buffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, (mg_vulkan_buffer_t*)buffer);

    vkMapMemory(context.device.handle, buffer->memory, 0, size, 0, &buffer->memory_mapped);

    return buffer;
}

void mg_vulkan_update_uniform_buffer(mg_vulkan_uniform_buffer_t *buffer, void *data, size_t size)
{
    memcpy(buffer->memory_mapped, data, size);
}