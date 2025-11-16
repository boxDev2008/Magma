#pragma once

#include "base.h"

#if MG_PLATFORM_WINDOWS

#include "../../renderer.h"

#define COBJMACROS
#include <d3d11.h>

typedef struct mg_d3d11_image
{
    ID3D11Resource *texture;
    ID3D11ShaderResourceView *view;
}
mg_d3d11_image;

typedef struct mg_d3d11_buffer
{
    ID3D11Buffer *buffer;
    bool is_cpu;
}
mg_d3d11_buffer;

typedef enum mg_d3d11_pipeline_type
{
    MG_D3D11_PIPELINE_TYPE_GRAPHICS,
    MG_D3D11_PIPELINE_TYPE_COMPUTE,
}
mg_d3d11_pipeline_type;

typedef struct mg_d3d11_pipeline
{
    mg_d3d11_pipeline_type type;
    ID3D11InputLayout *vertex_layout;
    ID3D11VertexShader *vertex_shader;
    ID3D11PixelShader *pixel_shader;
    ID3D11ComputeShader *compute_shader;
    ID3D11RasterizerState *raster_state;
    ID3D11DepthStencilState *depth_stencil_state;
    ID3D11BlendState *blend_state;
    D3D11_PRIMITIVE_TOPOLOGY primitive_topology;
    uint32_t layout_stride;
}
mg_d3d11_pipeline;

typedef struct mg_d3d11_render_pass
{
    ID3D11RenderTargetView *color_attachment;
    ID3D11DepthStencilView *depth_stencil_attachment;
    DXGI_FORMAT dsv_format;
}
mg_d3d11_render_pass;

typedef struct mg_d3d11_context
{
    mg_d3d11_pipeline *current_pipeline;
    ID3D11Device *device;
    ID3D11DeviceContext *context;
    IDXGISwapChain *swapchain;
    ID3D11DeviceContext *immediate_context;
    ID3D11RenderTargetView *target_view;

    ID3D11Buffer *constant_buffers[MG_CONFIG_MAX_BINDABLE_UNIFORMS];

    //ID3D11DepthStencilView* depth_stencil_view;
    //ID3D11Texture2D* depth_stencil_buffer;

    bool vsync;
}
mg_d3d11_context;

#endif