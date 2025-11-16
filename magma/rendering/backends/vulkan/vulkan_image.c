#include "vulkan_image.h"
#include "vulkan_buffer.h"
#include "vulkan_resources.h"
#include "vulkan_command_buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void mg_vulkan_allocate_image(uint32_t width, uint32_t height, VkImageType type, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *memory)
{
    VkImageCreateInfo image_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_info.imageType = type;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(vk_ctx.device.handle, &image_info, NULL, image);
    assert(result == VK_SUCCESS);
    
    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(vk_ctx.device.handle, *image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = mg_vulkan_find_memory_type(mem_requirements.memoryTypeBits, properties);

    result = vkAllocateMemory(vk_ctx.device.handle, &alloc_info, NULL, memory);
    assert(result == VK_SUCCESS);

    vkBindImageMemory(vk_ctx.device.handle, *image, *memory, 0);
}

void mg_vulkan_transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout)
{
    VkCommandBuffer command_buffer = mg_vulkan_begin_single_time_commands();

    VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    vkCmdPipelineBarrier(
        command_buffer,
        source_stage, destination_stage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
    );

    mg_vulkan_end_single_time_commands(command_buffer);
}

void mg_vulkan_copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer command_buffer = mg_vulkan_begin_single_time_commands();

    VkBufferImageCopy region = { 0 };
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = (VkOffset3D){0, 0, 0};
    region.imageExtent = (VkExtent3D){
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        command_buffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    mg_vulkan_end_single_time_commands(command_buffer);
}

mg_vulkan_image *mg_vulkan_create_image(mg_image_create_info *create_info)
{
    mg_vulkan_image *image = (mg_vulkan_image*)malloc(sizeof(mg_vulkan_image));

    const VkImageUsageFlagBits usage_flags =
        VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT |
        (uint32_t)create_info->usage;

    mg_vulkan_allocate_image(create_info->width, create_info->height, create_info->type, create_info->format,
        VK_IMAGE_TILING_OPTIMAL,
        usage_flags,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image->image, &image->memory);

    VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_info.image = image->image;
    view_info.viewType = (VkImageViewType)create_info->type;
    view_info.format = (VkFormat)create_info->format;
    view_info.subresourceRange.aspectMask =
        create_info->usage == MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT ?
        VK_IMAGE_ASPECT_DEPTH_BIT :
        VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    VkResult result = vkCreateImageView(vk_ctx.device.handle, &view_info, NULL, &image->view);
    assert(result == VK_SUCCESS);

    return image;
}

void mg_vulkan_destroy_image(mg_vulkan_image *image)
{
    mg_vulkan_free_resource((mg_vulkan_resource){.type = MG_VULKAN_RESOURCE_TYPE_IMAGE, .image = image});
}

void mg_vulkan_update_image(mg_vulkan_image *image, mg_image_update_info *write_info)
{
    VkDeviceSize image_size = write_info->width * write_info->height * 4;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    mg_vulkan_allocate_buffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &staging_buffer, &staging_memory);

    void* data;
    vkMapMemory(vk_ctx.device.handle, staging_memory, 0, image_size, 0, &data);
        memcpy(data, write_info->data, image_size);
    vkUnmapMemory(vk_ctx.device.handle, staging_memory);

    mg_vulkan_transition_image_layout(image->image, write_info->format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    mg_vulkan_copy_buffer_to_image(staging_buffer, image->image, write_info->width, write_info->height);
    mg_vulkan_transition_image_layout(image->image, write_info->format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(vk_ctx.device.handle, staging_buffer, NULL);
    vkFreeMemory(vk_ctx.device.handle, staging_memory, NULL);
}

void mg_vulkan_bind_image(mg_vulkan_image *image, VkSampler sampler, uint32_t binding)
{
    mg_vulkan_descriptor_cache* cache = &vk_ctx.descriptor_cache;
    if (cache->pending_image_binding_count >= MG_CONFIG_MAX_BINDABLE_IMAGES)
        return;
    
    cache->pending_image_bindings[cache->pending_image_binding_count].image_view = image->view;
    cache->pending_image_bindings[cache->pending_image_binding_count].sampler = sampler;
    cache->pending_image_bindings[cache->pending_image_binding_count].binding = binding;
    cache->pending_image_binding_count++;
}

VkSampler mg_vulkan_create_sampler(mg_sampler_create_info *create_info)
{
    VkSampler sampler;

    VkSamplerCreateInfo samplerInfo = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    samplerInfo.magFilter = (VkFilter)create_info->mag_filter;
    samplerInfo.minFilter = (VkFilter)create_info->min_filter;

    samplerInfo.addressModeU = (VkSamplerAddressMode)create_info->address_mode_u;
    samplerInfo.addressModeV = (VkSamplerAddressMode)create_info->address_mode_v;
    samplerInfo.addressModeW = (VkSamplerAddressMode)create_info->address_mode_w;

    samplerInfo.anisotropyEnable = VK_FALSE; // TODO (box): Make it customizable
    //samplerInfo.maxAnisotropy = vk_ctx.physical_device.properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkResult result = vkCreateSampler(vk_ctx.device.handle, &samplerInfo, NULL, &sampler);
    assert(result == VK_SUCCESS);

    return sampler;
}

void mg_vulkan_destroy_sampler(VkSampler sampler)
{
    mg_vulkan_free_resource((mg_vulkan_resource){.type = MG_VULKAN_RESOURCE_TYPE_SAMPLER, .sampler = sampler});
}