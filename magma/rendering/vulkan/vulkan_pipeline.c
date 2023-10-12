#include "vulkan_pipeline.h"
#include "vulkan_renderer.h"

#include <stdlib.h>
#include <assert.h>

VkShaderModule mg_vulkan_create_shader(const uint32_t *code, size_t size)
{
    VkShaderModuleCreateInfo create_info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    create_info.codeSize = size;
    create_info.pCode = code;

    VkShaderModule shader_module;
    VkResult result = vkCreateShaderModule(vulkan_context.device.handle, &create_info, NULL, &shader_module);
    assert(result == VK_SUCCESS);

    return shader_module;
}

mg_vulkan_pipeline_t *mg_vulkan_create_pipeline(mg_pipeline_create_info_t *create_info)
{
    mg_vulkan_pipeline_t *pipeline = (mg_vulkan_pipeline_t*)malloc(sizeof(mg_vulkan_pipeline_t));

    VkShaderModule vertex_shader_module = mg_vulkan_create_shader(create_info->vertex_shader->code, create_info->vertex_shader->code_size);
    VkShaderModule fragment_shader_module = mg_vulkan_create_shader(create_info->fragment_shader->code, create_info->fragment_shader->code_size);

    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vertex_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic_state.dynamicStateCount = 2;
    dynamic_state.pDynamicStates = dynamic_states;

    VkVertexInputBindingDescription binding_description;
    binding_description.binding = 0;
    binding_description.stride = create_info->vertex_layout.stride;
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    VkVertexInputAttributeDescription *attribute_descriptions =
        (VkVertexInputAttributeDescription*)malloc(create_info->vertex_layout.attribute_count * sizeof(VkVertexInputAttributeDescription));

    for (uint32_t i = 0; i < create_info->vertex_layout.attribute_count; i++)
    {
        attribute_descriptions[i].binding = 0;
        attribute_descriptions[i].location = create_info->vertex_layout.attributes[i].location;
        attribute_descriptions[i].format = create_info->vertex_layout.attributes[i].format;
        attribute_descriptions[i].offset = create_info->vertex_layout.attributes[i].offset;
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_info.vertexAttributeDescriptionCount = create_info->vertex_layout.attribute_count;
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions;

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = create_info->polygon_mode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = create_info->cull_mode;
    rasterizer.frontFace = create_info->front_face;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState color_blend_attachment = { 0 };
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = create_info->color_blend.blend_enabled;
    color_blend_attachment.srcColorBlendFactor = create_info->color_blend.src_color_blend_factor;
    color_blend_attachment.dstColorBlendFactor = create_info->color_blend.dst_color_blend_factor;
    color_blend_attachment.colorBlendOp = create_info->color_blend.color_blend_op;
    color_blend_attachment.srcAlphaBlendFactor = create_info->color_blend.src_alpha_blend_factor;
    color_blend_attachment.dstAlphaBlendFactor = create_info->color_blend.dst_alpha_blend_factor;
    color_blend_attachment.alphaBlendOp = create_info->color_blend.alpha_blend_op;

    VkPipelineColorBlendStateCreateInfo color_blending = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayout set_layouts[] = {
        vulkan_context.layouts.uniform_buffer_layout,
        vulkan_context.layouts.image_sampler_layout
    };

    pipeline_layout_info.pSetLayouts = set_layouts;
    pipeline_layout_info.setLayoutCount = 2;

    if (create_info->has_push_constants)
    {
        VkPushConstantRange push_constant = { 0 };
        push_constant.offset = 0;
        push_constant.size = create_info->push_constants_size;
        push_constant.stageFlags = VK_SHADER_STAGE_ALL;

        pipeline_layout_info.pushConstantRangeCount = 1;
        pipeline_layout_info.pPushConstantRanges = &push_constant;
    }

    VkResult result = vkCreatePipelineLayout(vulkan_context.device.handle, &pipeline_layout_info, NULL, &pipeline->pipeline_layout);
    assert(result == VK_SUCCESS);

    VkGraphicsPipelineCreateInfo pipeline_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;

    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = &dynamic_state;

    pipeline_info.layout = pipeline->pipeline_layout;
    pipeline_info.renderPass = create_info->render_pass.internal_data ?
        create_info->render_pass.internal_data : vulkan_context.render_pass;
    pipeline_info.subpass = 0;

    result = vkCreateGraphicsPipelines(vulkan_context.device.handle, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &pipeline->pipeline);
    assert(result == VK_SUCCESS);

    free(attribute_descriptions);

    vkDestroyShaderModule(vulkan_context.device.handle, vertex_shader_module, NULL);
    vkDestroyShaderModule(vulkan_context.device.handle, fragment_shader_module, NULL);

    return pipeline;
}

void mg_vulkan_destroy_pipeline(mg_vulkan_pipeline_t *pipeline)
{
    vkDestroyPipeline(vulkan_context.device.handle, pipeline->pipeline, NULL);
    vkDestroyPipelineLayout(vulkan_context.device.handle, pipeline->pipeline_layout, NULL);
    free(pipeline);
}

void mg_vulkan_bind_pipeline(mg_vulkan_pipeline_t *pipeline)
{
    vkCmdBindPipeline(vulkan_context.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
}