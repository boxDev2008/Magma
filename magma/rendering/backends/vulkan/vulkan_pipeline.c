#include "vulkan_pipeline.h"
#include "vulkan_renderer.h"
#include "vulkan_resources.h"

#include <stdlib.h>
#include <assert.h>

static VkShaderModule mg_vulkan_create_shader(const uint32_t *code, size_t size)
{
    VkShaderModuleCreateInfo create_info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    create_info.codeSize = size;
    create_info.pCode = code;

    VkShaderModule shader_module;
    VkResult result = vkCreateShaderModule(vk_ctx.device.handle, &create_info, NULL, &shader_module);
    assert(result == VK_SUCCESS);

    return shader_module;
}

static void mg_vulkan_fill_graphics_pipeline(mg_vulkan_pipeline *pipeline, const mg_pipeline_create_info *create_info)
{
    VkShaderModule vertex_shader_module = mg_vulkan_create_shader(create_info->shader.vertex.code, create_info->shader.vertex.size);
    VkShaderModule fragment_shader_module = mg_vulkan_create_shader(create_info->shader.fragment.code, create_info->shader.fragment.size);

    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vertex_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    static const VkDynamicState dynamic_states[] = {
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
    
    VkVertexInputAttributeDescription attribute_descriptions[MG_CONFIG_MAX_VERTEX_ATTRIBUTES];

    uint32_t attribute_count;
    for (attribute_count = 0; attribute_count < MG_CONFIG_MAX_VERTEX_ATTRIBUTES
        && create_info->vertex_layout.attributes[attribute_count].format; attribute_count++)
    {
        VkVertexInputAttributeDescription *desc = &attribute_descriptions[attribute_count];
        desc->location = create_info->vertex_layout.attributes[attribute_count].location;
        desc->format = (VkFormat)create_info->vertex_layout.attributes[attribute_count].format;
        desc->offset = create_info->vertex_layout.attributes[attribute_count].offset;
        desc->binding = 0;
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_info.vertexAttributeDescriptionCount = attribute_count;
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions;

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    input_assembly.topology = (VkPrimitiveTopology)create_info->primitive_topology;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = (VkPolygonMode)create_info->polygon_mode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = create_info->cull_mode;
    rasterizer.frontFace = (VkFrontFace)create_info->front_face;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    if (create_info->depth_stencil.depth_compare_op != MG_COMPARE_OP_NEVER)
    {
        depth_stencil.depthTestEnable = true;
        depth_stencil.depthWriteEnable = create_info->depth_stencil.depth_write_enabled;
        depth_stencil.depthCompareOp = (VkCompareOp)create_info->depth_stencil.depth_compare_op;
    }
    depth_stencil.stencilTestEnable = create_info->depth_stencil.stencil_test_enabled;

    VkPipelineColorBlendAttachmentState color_blend_attachment = { 0 };
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = create_info->color_blend.blend_enabled;
    if (create_info->color_blend.blend_enabled)
    {
        color_blend_attachment.srcColorBlendFactor = (VkBlendFactor)create_info->color_blend.src_color_blend_factor;
        color_blend_attachment.dstColorBlendFactor = (VkBlendFactor)create_info->color_blend.dst_color_blend_factor;
        color_blend_attachment.colorBlendOp = (VkBlendOp)create_info->color_blend.color_blend_op;
        color_blend_attachment.srcAlphaBlendFactor = (VkBlendFactor)create_info->color_blend.src_alpha_blend_factor;
        color_blend_attachment.dstAlphaBlendFactor = (VkBlendFactor)create_info->color_blend.dst_alpha_blend_factor;
        color_blend_attachment.alphaBlendOp = (VkBlendOp)create_info->color_blend.alpha_blend_op;
    }

    VkPipelineColorBlendStateCreateInfo color_blending = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    const VkDescriptorSetLayout set_layouts[] = {
        vk_ctx.layouts.scratch_buffer_layout,
        vk_ctx.layouts.image_sampler_layout,
    };

    pipeline_layout_info.pSetLayouts = set_layouts;
    pipeline_layout_info.setLayoutCount = 2;

    VkResult result = vkCreatePipelineLayout(vk_ctx.device.handle, &pipeline_layout_info, NULL, &pipeline->pipeline_layout);
    assert(result == VK_SUCCESS);

    VkGraphicsPipelineCreateInfo pipeline_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;

    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = &dynamic_state;

    pipeline_info.layout = pipeline->pipeline_layout;
    pipeline_info.renderPass = create_info->render_pass ?
        ((mg_vulkan_render_pass*)create_info->render_pass)->render_pass : vk_ctx.default_render_pass;
    pipeline_info.subpass = 0;

    result = vkCreateGraphicsPipelines(vk_ctx.device.handle, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &pipeline->pipeline);
    assert(result == VK_SUCCESS);

    vkDestroyShaderModule(vk_ctx.device.handle, vertex_shader_module, NULL);
    vkDestroyShaderModule(vk_ctx.device.handle, fragment_shader_module, NULL);

    pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
}

static void mg_vulkan_fill_compute_pipeline(mg_vulkan_pipeline *pipeline, const mg_pipeline_create_info *create_info)
{
    VkShaderModule compute_shader = mg_vulkan_create_shader(
        create_info->shader.compute.code,
        create_info->shader.compute.size
    );

    const VkDescriptorSetLayout set_layouts[] = {
        vk_ctx.layouts.scratch_buffer_layout,
        vk_ctx.layouts.image_sampler_layout
    };

    VkPipelineLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 2,
        .pSetLayouts = set_layouts
    };

    VkResult result = vkCreatePipelineLayout(
        vk_ctx.device.handle, &layout_info, NULL, &pipeline->pipeline_layout);
    assert(result == VK_SUCCESS);

    VkPipelineShaderStageCreateInfo stage = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = compute_shader,
        .pName = "main"
    };

    VkComputePipelineCreateInfo compute_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = stage,
        .layout = pipeline->pipeline_layout
    };

    result = vkCreateComputePipelines(
        vk_ctx.device.handle, VK_NULL_HANDLE, 1, &compute_info, NULL, &pipeline->pipeline);
    assert(result == VK_SUCCESS);

    vkDestroyShaderModule(vk_ctx.device.handle, compute_shader, NULL);
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
}

mg_vulkan_pipeline *mg_vulkan_create_pipeline(const mg_pipeline_create_info *create_info)
{
    mg_vulkan_pipeline *pipeline = (mg_vulkan_pipeline*)malloc(sizeof(mg_vulkan_pipeline));
    if (create_info->shader.compute.size)
        mg_vulkan_fill_compute_pipeline(pipeline, create_info);
    else mg_vulkan_fill_graphics_pipeline(pipeline, create_info);
    return pipeline;
}

void mg_vulkan_destroy_pipeline(mg_vulkan_pipeline *pipeline)
{
    mg_vulkan_free_resource((mg_vulkan_resource){.type = MG_VULKAN_RESOURCE_TYPE_PIPELINE, .pipeline = pipeline});
}

void mg_vulkan_bind_pipeline(mg_vulkan_pipeline *pipeline)
{
    vkCmdBindPipeline(vk_ctx.command_buffer, pipeline->bind_point, pipeline->pipeline);
    vk_ctx.current_pipeline = pipeline;
}