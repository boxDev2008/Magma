#include "vulkan_image.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void mg_vulkan_allocate_image(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *memory)
{
    VkImageCreateInfo image_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_info.imageType = VK_IMAGE_TYPE_2D;
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

    VkResult result = vkCreateImage(context.device.handle, &image_info, NULL, image);
    assert(result == VK_SUCCESS);
    
    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(context.device.handle, *image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = mg_vulkan_find_memory_type(mem_requirements.memoryTypeBits, properties);

    result = vkAllocateMemory(context.device.handle, &alloc_info, NULL, memory);
    assert(result == VK_SUCCESS);

    vkBindImageMemory(context.device.handle, *image, *memory, 0);
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

mg_vulkan_texture_image_t *mg_vulkan_create_texture_image(mg_texture_image_create_info_t *create_info)
{
    mg_vulkan_texture_image_t *texture_image = (mg_vulkan_texture_image_t*)malloc(sizeof(mg_vulkan_texture_image_t));

    mg_vulkan_allocate_image(create_info->extent.x, create_info->extent.y, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture_image->image, &texture_image->memory);

    return texture_image;
}

void mg_vulkan_write_texture_image(mg_vulkan_texture_image_t *texture_image, mg_texture_image_write_info_t *write_info)
{
    VkDeviceSize image_size = write_info->extent.x * write_info->extent.y * 4;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;

    mg_vulkan_allocate_buffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &staging_buffer, &staging_memory);

    void* data;
    vkMapMemory(context.device.handle, staging_memory, 0, image_size, 0, &data);
        memcpy(data, write_info->data, image_size);
    vkUnmapMemory(context.device.handle, staging_memory);

    mg_vulkan_transition_image_layout(texture_image->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    mg_vulkan_copy_buffer_to_image(staging_buffer, texture_image->image, write_info->extent.x, write_info->extent.y);
    mg_vulkan_transition_image_layout(texture_image->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(context.device.handle, staging_buffer, NULL);
    vkFreeMemory(context.device.handle, staging_memory, NULL);
}

void mg_vulkan_destroy_texture_image(mg_vulkan_texture_image_t *texture_image)
{
    vkDestroyImage(context.device.handle, texture_image->image, NULL);
    vkFreeMemory(context.device.handle, texture_image->memory, NULL);

    free(texture_image);
}

VkImageView mg_vulkan_create_texture_view(mg_vulkan_texture_image_t *texture_image)
{
    VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_info.image = texture_image->image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    VkImageView view;

    VkResult result = vkCreateImageView(context.device.handle, &view_info, NULL, &view);
    assert(result == VK_SUCCESS);

    return view;
}

void mg_vulkan_destroy_texture_view(VkImageView view)
{
    vkDestroyImageView(context.device.handle, view, NULL);
}

VkSampler mg_vulkan_create_sampler(mg_sampler_create_info_t *create_info)
{
    VkSampler sampler;

    VkSamplerCreateInfo samplerInfo = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    samplerInfo.magFilter = create_info->mag_filter;
    samplerInfo.minFilter = create_info->min_filter;

    samplerInfo.addressModeU = create_info->address_mode_u;
    samplerInfo.addressModeV = create_info->address_mode_v;
    samplerInfo.addressModeW = create_info->address_mode_w;

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = context.physical_device.properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkResult result = vkCreateSampler(context.device.handle, &samplerInfo, NULL, &sampler);
    assert(result == VK_SUCCESS);

    return sampler;
}

void mg_vulkan_destroy_sampler(VkSampler sampler)
{
    vkDestroySampler(context.device.handle, sampler, NULL);
}

VkFramebuffer mg_vulkan_create_framebuffer(mg_framebuffer_create_info_t *create_info)
{
    VkFramebufferCreateInfo framebuffer_create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebuffer_create_info.attachmentCount = 1;

    framebuffer_create_info.pAttachments = &create_info->texture_view.internal_data;
    framebuffer_create_info.renderPass = create_info->render_pass.internal_data;
    framebuffer_create_info.width = create_info->extent.x;
    framebuffer_create_info.height = create_info->extent.y;
    framebuffer_create_info.layers = 1;

    VkFramebuffer framebuffer;

    VkResult result = vkCreateFramebuffer(context.device.handle, &framebuffer_create_info, NULL, &framebuffer);
    assert(result == VK_SUCCESS);

    return framebuffer;
}

void mg_vulkan_destroy_framebuffer(VkFramebuffer framebuffer)
{
    vkDestroyFramebuffer(context.device.handle, framebuffer, NULL);
}