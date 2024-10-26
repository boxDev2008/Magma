#include "d3d11_renderer.h"

#if MG_PLATFORM_WINDOWS

#include "d3d11_utils.h"
#include <guiddef.h>
#include <math.h>

#pragma comment (lib, "dxgi")
#pragma comment (lib, "d3d11")
#pragma comment (lib, "d3dcompiler")
#pragma comment (lib, "dxguid.lib")

mg_d3d11_context d3d11_context;

void mg_d3d11_renderer_initialize(mg_lowl_renderer_init_info *init_info)
{
    mg_swapchain_config_info *swapchain_info = init_info->swapchain_config_info;
    DXGI_SWAP_CHAIN_DESC sd = { 0 };
    sd.BufferCount = 1;
    sd.BufferDesc.Width = swapchain_info->width;
    sd.BufferDesc.Height = swapchain_info->height;
    sd.BufferDesc.Format = mg_d3d11_get_pixel_format(swapchain_info->format);
    //sd.BufferDesc.RefreshRate.Numerator = 60;
    //sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = mg_platform_win32_get_handler(init_info->platform);
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &d3d11_context.swapchain,
        &d3d11_context.device,
        NULL,
        &d3d11_context.immediate_context);

    ID3D11Texture2D* pBackBuffer = NULL;
    IDXGISwapChain_GetBuffer(d3d11_context.swapchain, 0, &IID_ID3D11Texture2D, (void**)&pBackBuffer);
    ID3D11Device_CreateRenderTargetView(d3d11_context.device, (ID3D11Resource*)pBackBuffer, NULL, &d3d11_context.target_view);
    ID3D11Buffer_Release(pBackBuffer);

    switch (swapchain_info->present_mode)
    {
        case MG_PRESENT_MODE_IMMEDIATE:
        case MG_PRESENT_MODE_MAILBOX:
            d3d11_context.vsync = false;
            break;
        default:
            d3d11_context.vsync = true;
            break;
    }
}

void mg_d3d11_renderer_shutdown(void)
{
    ID3D11DeviceContext_ClearState(d3d11_context.immediate_context);
    ID3D11RenderTargetView_Release(d3d11_context.target_view);
    IDXGISwapChain_Release(d3d11_context.swapchain);
    ID3D11DeviceContext_Release(d3d11_context.immediate_context);
    ID3D11Device_Release(d3d11_context.device);
}

void mg_d3d11_renderer_begin_frame(void)
{

}

void mg_d3d11_renderer_end_frame(void)
{

}

void mg_d3d11_renderer_present_frame(void)
{
    IDXGISwapChain_Present(d3d11_context.swapchain, d3d11_context.vsync, 0);
}

void mg_d3d11_renderer_wait(void)
{
    
}

void mg_d3d11_renderer_configure_swapchain(mg_swapchain_config_info *config_info)
{
    ID3D11RenderTargetView_Release(d3d11_context.target_view);
    IDXGISwapChain_ResizeBuffers(
        d3d11_context.swapchain,
        0,
        config_info->width,
        config_info->height,
        mg_d3d11_get_pixel_format(config_info->format),
        0
    );
    ID3D11Texture2D* pBackBuffer = NULL;
    IDXGISwapChain_GetBuffer(d3d11_context.swapchain, 0, &IID_ID3D11Texture2D, (void**)&pBackBuffer);
    ID3D11Device_CreateRenderTargetView(d3d11_context.device, (ID3D11Resource*)pBackBuffer, NULL, &d3d11_context.target_view);
    ID3D11Buffer_Release(pBackBuffer);

    switch (config_info->present_mode)
    {
        case MG_PRESENT_MODE_IMMEDIATE:
        case MG_PRESENT_MODE_MAILBOX:
            d3d11_context.vsync = false;
            break;
        default:
            d3d11_context.vsync = true;
            break;
    }
}

void mg_d3d11_renderer_viewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    D3D11_VIEWPORT vp = { 0 };
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = x;
    vp.TopLeftY = y;
    ID3D11DeviceContext_RSSetViewports(d3d11_context.immediate_context, 1, &vp);
}

void mg_d3d11_renderer_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    D3D11_RECT rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;
    ID3D11DeviceContext_RSSetScissorRects(d3d11_context.immediate_context, 1, &rect);
}

void mg_d3d11_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    ID3D11DeviceContext_Draw(d3d11_context.immediate_context, vertex_count, first_vertex);
}

void mg_d3d11_renderer_draw_indexed(uint32_t index_count, uint32_t first_index)
{
    ID3D11DeviceContext_DrawIndexed(d3d11_context.immediate_context, index_count, first_index, 0);
}

void mg_d3d11_renderer_push_constants(uint32_t size, void *data)
{
    ID3D11DeviceContext_UpdateSubresource(d3d11_context.immediate_context,
        d3d11_context.binds.pipeline->push_constant_buffer, 0, NULL, data, 0, 0);
}

#endif