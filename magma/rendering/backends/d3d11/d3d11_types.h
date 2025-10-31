#pragma once

#include "base.h"

#if MG_PLATFORM_WINDOWS

#include "../../renderer.h"

#define COBJMACROS
#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>

typedef struct mg_d3d11_image
{
    ID3D11Texture2D *texture;
    ID3D11ShaderResourceView *view;
}
mg_d3d11_image;

typedef struct mg_d3d11_image_array
{
    ID3D11ShaderResourceView *views[MG_CONFIG_MAX_BINDABLE_IMAGES];
    ID3D11SamplerState *samplers[MG_CONFIG_MAX_BINDABLE_IMAGES];
    uint32_t count;
}
mg_d3d11_image_array;

typedef struct mg_d3d11_pipeline
{
    ID3D11InputLayout *vertex_layout;
    ID3D11VertexShader *vertex_shader;
    ID3D11PixelShader *pixel_shader;
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
}
mg_d3d11_render_pass;

typedef struct mg_d3d11_context
{
    struct
    {
        mg_d3d11_pipeline *pipeline;
    }
    binds;

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