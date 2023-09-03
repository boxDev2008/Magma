#include "vulkan_descriptor_set.h"
#include "vulkan_renderer.h"

#include <assert.h>

VkDescriptorSetLayout mg_vulkan_create_descriptor_set_layout(mg_descriptor_set_layout_create_info_t *create_info)
{
    VkDescriptorSetLayout descriptor_set_layout;

    VkDescriptorSetLayoutBinding layout_bindings[create_info->descriptor_count];

    for (uint32_t i = 0; i < create_info->descriptor_count; i++)
    {
        layout_bindings[i].binding = create_info->descriptors[i].binding;
        layout_bindings[i].descriptorType = create_info->descriptors[i].type;
        layout_bindings[i].descriptorCount = 1;
        layout_bindings[i].stageFlags = create_info->descriptors[i].stage;
        layout_bindings[i].pImmutableSamplers = NULL;
    }

    VkDescriptorSetLayoutCreateInfo layout_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    layout_info.bindingCount = create_info->descriptor_count;
    layout_info.pBindings = layout_bindings;

    VkResult result = vkCreateDescriptorSetLayout(context.device.handle, &layout_info, NULL, &descriptor_set_layout);
    assert(result == VK_SUCCESS);

    return descriptor_set_layout;
}

void mg_vulkan_destroy_descriptor_set_layout(VkDescriptorSetLayout descriptor_set_layout)
{
    vkDestroyDescriptorSetLayout(context.device.handle, descriptor_set_layout, NULL);
}

VkDescriptorSet mg_vulkan_create_descriptor_set(mg_descriptor_set_create_info_t *create_info)
{
    VkDescriptorSet descriptor_set;

    VkDescriptorSetAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    alloc_info.descriptorPool = context.descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = (VkDescriptorSetLayout*)create_info->layouts;

    VkResult result = vkAllocateDescriptorSets(context.device.handle, &alloc_info, &descriptor_set);
    assert(result == VK_SUCCESS);

    return descriptor_set;
}

void mg_vulkan_update_descriptor_set(VkDescriptorSet descriptor_set, mg_descriptor_write_t *descriptor_write)
{
    VkWriteDescriptorSet write = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    write.dstSet = descriptor_set;
    write.dstBinding = descriptor_write->binding;
    write.dstArrayElement = 0;

    write.descriptorType = (VkDescriptorType)descriptor_write->descriptor_type;
    write.descriptorCount = 1;

    VkDescriptorBufferInfo buffer_info;
    VkDescriptorImageInfo image_info;

    if (descriptor_write->buffer_info)
    {
        mg_vulkan_buffer_t *buffer = (mg_vulkan_buffer_t*)descriptor_write->buffer_info->buffer.internal_data;
        buffer_info.buffer = buffer->buffer;
        buffer_info.offset = descriptor_write->buffer_info->offset;
        buffer_info.range = descriptor_write->buffer_info->range;

        write.pBufferInfo = &buffer_info;
    }

    if (descriptor_write->image_info)
    {
        mg_vulkan_texture_image_t *image = (mg_vulkan_texture_image_t*)descriptor_write->image_info->image.internal_data;
        image_info.sampler = descriptor_write->image_info->sampler.internal_data;
        image_info.imageView = image->view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        write.pImageInfo = &image_info;
    }

    vkUpdateDescriptorSets(context.device.handle, 1, &write, 0, NULL);
}

void mg_vulkan_destroy_descriptor_set(VkDescriptorSet descriptor_set)
{
    vkFreeDescriptorSets(context.device.handle, context.descriptor_pool, 1, &descriptor_set);
}

void mg_vulkan_bind_descriptor_set(VkDescriptorSet descriptor_set, mg_vulkan_program_t *program, uint32_t set_index)
{
    vkCmdBindDescriptorSets(context.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program->pipeline_layout, set_index, 1, &descriptor_set, 0, NULL);
}