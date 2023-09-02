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

void mg_vulkan_allocate_buffer(size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *memory)
{
    VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(context.device.handle, &buffer_info, NULL, buffer);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(context.device.handle, *buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = mg_vulkan_find_memory_type(mem_requirements.memoryTypeBits, properties);
    
    result = vkAllocateMemory(context.device.handle, &alloc_info, NULL, memory);
    assert(result == VK_SUCCESS);

    vkBindBufferMemory(context.device.handle, *buffer, *memory, 0);
}

void mg_vulkan_copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
    VkCommandBuffer command_buffer = mg_vulkan_begin_single_time_commands();

    VkBufferCopy copy_region = { 0 };
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    mg_vulkan_end_single_time_commands(command_buffer);
}

mg_vulkan_buffer_t *mg_vulkan_create_buffer(mg_buffer_create_info_t *create_info)
{
    mg_vulkan_buffer_t *buffer = (mg_vulkan_buffer_t*)malloc(sizeof(mg_vulkan_buffer_t));
    buffer->mapped_at_creation = create_info->mapped_at_creation;
    buffer->frequency = create_info->frequency;

    if (create_info->frequency == MG_BUFFER_UPDATE_FREQUENCY_STATIC) // Puts buffer on the GPU
    {
        mg_vulkan_allocate_buffer(create_info->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &buffer->staging_buffer, &buffer->staging_memory);

        mg_vulkan_allocate_buffer(create_info->size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &buffer->buffer, &buffer->memory);

        if (create_info->mapped_at_creation)
            vkMapMemory(context.device.handle, buffer->staging_memory, 0, create_info->size, 0, &buffer->data);
    }
    else // Puts buffer on the CPU
    {
        mg_vulkan_allocate_buffer(create_info->size, (VkBufferUsageFlags)create_info->usage,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &buffer->buffer, &buffer->memory);

        if (create_info->mapped_at_creation)
            vkMapMemory(context.device.handle, buffer->memory, 0, create_info->size, 0, &buffer->data);
    }

    return buffer;
}

void mg_vulkan_update_buffer(mg_vulkan_buffer_t *buffer, mg_buffer_update_info_t *update_info)
{
    if (buffer->frequency == MG_BUFFER_UPDATE_FREQUENCY_STATIC)
    {
        if (buffer->mapped_at_creation)
            memcpy(buffer->data, update_info->data, update_info->size);
        else
        {
            vkMapMemory(context.device.handle, buffer->staging_memory, 0, update_info->size, 0, &buffer->data);
            memcpy(buffer->data, update_info->data, update_info->size);
            vkUnmapMemory(context.device.handle, buffer->staging_memory);
        }

        mg_vulkan_copy_buffer(buffer->staging_buffer, buffer->buffer, update_info->size);
    }
    else
    {
        if (buffer->mapped_at_creation)
            memcpy(buffer->data, update_info->data, update_info->size);
        else
        {
            vkMapMemory(context.device.handle, buffer->memory, 0, update_info->size, 0, &buffer->data);
            memcpy(buffer->data, update_info->data, update_info->size);
            vkUnmapMemory(context.device.handle, buffer->memory);
        }
    }
}

void mg_vulkan_destroy_buffer(mg_vulkan_buffer_t *buffer)
{
    vkDestroyBuffer(context.device.handle, buffer->buffer, NULL);
    vkFreeMemory(context.device.handle, buffer->memory, NULL);
}

void mg_vulkan_bind_vertex_buffer(mg_vulkan_buffer_t *buffer)
{
    VkBuffer vertex_buffers[] = {buffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(context.command_buffer, 0, 1, vertex_buffers, offsets);
}

void mg_vulkan_bind_index_buffer(mg_vulkan_buffer_t *buffer, mg_index_type_t index_type)
{
    vkCmdBindIndexBuffer(context.command_buffer, buffer->buffer, 0, index_type);
}