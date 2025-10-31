#include "d3d11_pipeline.h"

#if MG_PLATFORM_WINDOWS

#include "d3d11_utils.h"
#include "d3d11_buffer.h"

#include <d3dcommon.h>

#include <stdio.h>

mg_d3d11_pipeline *mg_d3d11_create_pipeline(mg_pipeline_create_info *create_info)
{
    mg_d3d11_pipeline *pipeline = (mg_d3d11_pipeline*)malloc(sizeof(mg_d3d11_pipeline));

    ID3DBlob* vs_blob = NULL;
    ID3DBlob* ps_blob = NULL;
    ID3DBlob* error_blob = NULL;

    mg_shader_source *vs = &create_info->shader.vertex;
    mg_shader_source *fg = &create_info->shader.fragment;

    D3DCompile(vs->code, vs->size, NULL, NULL, NULL,
        "main", "vs_5_0", 0, 0, &vs_blob, &error_blob
    );

    D3DCompile(fg->code, fg->size, NULL, NULL, NULL,
        "main", "ps_5_0", 0, 0, &ps_blob, &error_blob
    );

    if (error_blob)
    {
        printf("%s\n", (char*)ID3D10Blob_GetBufferPointer(error_blob));
        ID3D10Blob_Release(error_blob);
    }

    void *vs_buffer_ptr = ID3D10Blob_GetBufferPointer(vs_blob);
    size_t vs_buffer_size = ID3D10Blob_GetBufferSize(vs_blob);
    ID3D11Device_CreateVertexShader(
        d3d11_ctx.device,
        vs_buffer_ptr,
        vs_buffer_size,
        NULL,
        &pipeline->vertex_shader
    );

    ID3D11Device_CreatePixelShader(
        d3d11_ctx.device,
        ID3D10Blob_GetBufferPointer(ps_blob),
        ID3D10Blob_GetBufferSize(ps_blob),
        NULL,
        &pipeline->pixel_shader
    );

    const uint32_t attribute_count = create_info->vertex_layout.attribute_count;

	if (attribute_count)
	{
        D3D11_INPUT_ELEMENT_DESC layout[MG_CONFIG_MAX_VERTEX_ATTRIBUTES];
		for (uint32_t i = 0; i < attribute_count; i++)
		{
			mg_vertex_attribute_info *attribute = &create_info->vertex_layout.attributes[i];
			layout[i].SemanticName = "TEXCOORD";
			layout[i].SemanticIndex = attribute->location;
			layout[i].Format = mg_d3d11_get_vertex_format(attribute->format); 
			layout[i].AlignedByteOffset = attribute->offset;
			layout[i].InputSlot = 0;
			layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			layout[i].InstanceDataStepRate = 0;
		}

		ID3D11Device_CreateInputLayout(
			d3d11_ctx.device,
			layout,
			attribute_count,
			vs_buffer_ptr,
			vs_buffer_size,
			&pipeline->vertex_layout
		);
	}
	else
		pipeline->vertex_layout = NULL;

    D3D11_RASTERIZER_DESC rasterDesc = { 0 };
    rasterDesc.FillMode = mg_d3d11_get_fill_mode(create_info->polygon_mode);
    rasterDesc.CullMode = mg_d3d11_get_cull_mode(create_info->cull_mode);
    rasterDesc.FrontCounterClockwise = create_info->front_face == MG_FRONT_FACE_CCW;
    rasterDesc.DepthClipEnable = TRUE;
    rasterDesc.ScissorEnable = TRUE;
    ID3D11Device_CreateRasterizerState(d3d11_ctx.device, &rasterDesc, &pipeline->raster_state);

    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = { 0 };
    depthStencilStateDesc.DepthEnable = create_info->depth_stencil.depth_test_enabled;
    depthStencilStateDesc.StencilEnable = create_info->depth_stencil.stencil_test_enabled;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = mg_d3d11_get_comparison_func(create_info->depth_stencil.depth_compare_op);
    ID3D11Device_CreateDepthStencilState(d3d11_ctx.device, &depthStencilStateDesc, &pipeline->depth_stencil_state);

    D3D11_BLEND_DESC blendDesc = { 0 };
    blendDesc.RenderTarget[0].BlendEnable = create_info->color_blend.blend_enabled;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (create_info->color_blend.blend_enabled)
    {
        blendDesc.RenderTarget[0].SrcBlend = mg_d3d11_get_blend_factor(create_info->color_blend.src_color_blend_factor);
        blendDesc.RenderTarget[0].DestBlend = mg_d3d11_get_blend_factor(create_info->color_blend.dst_color_blend_factor);
        blendDesc.RenderTarget[0].BlendOp = mg_d3d11_get_blend_op(create_info->color_blend.color_blend_op);
        blendDesc.RenderTarget[0].SrcBlendAlpha = mg_d3d11_get_blend_factor(create_info->color_blend.src_alpha_blend_factor);
        blendDesc.RenderTarget[0].DestBlendAlpha = mg_d3d11_get_blend_factor(create_info->color_blend.dst_alpha_blend_factor);
        blendDesc.RenderTarget[0].BlendOpAlpha = mg_d3d11_get_blend_op(create_info->color_blend.alpha_blend_op);;
    }
    ID3D11Device_CreateBlendState(d3d11_ctx.device, &blendDesc, &pipeline->blend_state);

    pipeline->layout_stride = create_info->vertex_layout.stride;
    pipeline->primitive_topology = mg_d3d11_get_primitive_topology(create_info->primitive_topology);
    
    ID3D10Blob_Release(vs_blob);
    ID3D10Blob_Release(ps_blob);

    return pipeline;
}

void mg_d3d11_destroy_pipeline(mg_d3d11_pipeline *pipeline)
{
    ID3D11VertexShader_Release(pipeline->vertex_shader);
    ID3D11PixelShader_Release(pipeline->pixel_shader);
    ID3D11RasterizerState_Release(pipeline->raster_state);
    ID3D11DepthStencilState_Release(pipeline->depth_stencil_state);
    ID3D11BlendState_Release(pipeline->blend_state);
	if (pipeline->vertex_layout)
    	ID3D11InputLayout_Release(pipeline->vertex_layout);
    free(pipeline);
}

void mg_d3d11_bind_pipeline(mg_d3d11_pipeline *pipeline)
{
    ID3D11DeviceContext *context = d3d11_ctx.immediate_context;
    ID3D11DeviceContext_VSSetShader(context, pipeline->vertex_shader, NULL, 0);
    ID3D11DeviceContext_PSSetShader(context, pipeline->pixel_shader, NULL, 0);

    ID3D11DeviceContext_IASetInputLayout(context, pipeline->vertex_layout);
    ID3D11DeviceContext_IASetPrimitiveTopology(context, pipeline->primitive_topology);

    ID3D11DeviceContext_RSSetState(context, pipeline->raster_state);
    ID3D11DeviceContext_OMSetDepthStencilState(context, pipeline->depth_stencil_state, 0);

    const float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    ID3D11DeviceContext_OMSetBlendState(context, pipeline->blend_state, blend_factor, 0xffffffff);

    d3d11_ctx.binds.pipeline = pipeline;
}

#endif