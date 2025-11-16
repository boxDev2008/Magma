#include "d3d11_renderer.h"

#if MG_PLATFORM_WINDOWS

#include "d3d11_utils.h"

#pragma comment (lib, "dxgi")
#pragma comment (lib, "d3d11")
#pragma comment (lib, "d3dcompiler")
#pragma comment (lib, "dxguid.lib")

mg_d3d11_context d3d11_ctx;

void mg_d3d11_renderer_initialize(mgfx_init_info *init_info)
{
    mg_swapchain_config_info *swapchain_info = init_info->swapchain_config_info;
    DXGI_SWAP_CHAIN_DESC sd = { 0 };
    sd.BufferCount = 1;
    sd.BufferDesc.Width = swapchain_info->width;
    sd.BufferDesc.Height = swapchain_info->height;
    sd.BufferDesc.Format = mg_d3d11_get_pixel_format(swapchain_info->format);
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = ((mg_win32_platform*)mg_platform_get_handle())->hwnd;
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
        &d3d11_ctx.swapchain,
        &d3d11_ctx.device,
        NULL,
        &d3d11_ctx.immediate_context);

    ID3D11Texture2D* pBackBuffer = NULL;
    IDXGISwapChain_GetBuffer(d3d11_ctx.swapchain, 0, &IID_ID3D11Texture2D, (void**)&pBackBuffer);
    ID3D11Device_CreateRenderTargetView(d3d11_ctx.device, (ID3D11Resource*)pBackBuffer, NULL, &d3d11_ctx.target_view);
    ID3D11Buffer_Release(pBackBuffer);

    for (uint32_t i = 0; i < MG_CONFIG_MAX_BINDABLE_UNIFORMS; i++)
    {
        D3D11_BUFFER_DESC buffer_desc = { 0 };
        buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
        buffer_desc.ByteWidth = MG_CONFIG_MAX_UNIFORM_UPDATE_SIZE;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        ID3D11Device_CreateBuffer(d3d11_ctx.device, &buffer_desc, NULL, &d3d11_ctx.constant_buffers[i]);
    }

    d3d11_ctx.vsync = swapchain_info->vsync;
}

void mg_d3d11_renderer_shutdown(void)
{
    for (uint32_t i = 0; i < MG_CONFIG_MAX_BINDABLE_UNIFORMS; i++)
        ID3D11Buffer_Release(d3d11_ctx.constant_buffers[i]);
    ID3D11DeviceContext_ClearState(d3d11_ctx.immediate_context);
    ID3D11RenderTargetView_Release(d3d11_ctx.target_view);
    IDXGISwapChain_Release(d3d11_ctx.swapchain);
    ID3D11DeviceContext_Release(d3d11_ctx.immediate_context);
    ID3D11Device_Release(d3d11_ctx.device);
}

void mg_d3d11_renderer_begin(void)
{
    ID3D11DeviceContext_VSSetConstantBuffers(d3d11_ctx.immediate_context, 0, MG_CONFIG_MAX_BINDABLE_UNIFORMS, d3d11_ctx.constant_buffers);
    ID3D11DeviceContext_PSSetConstantBuffers(d3d11_ctx.immediate_context, 0, MG_CONFIG_MAX_BINDABLE_UNIFORMS, d3d11_ctx.constant_buffers);
}

void mg_d3d11_renderer_end(void)
{
    IDXGISwapChain_Present(d3d11_ctx.swapchain, d3d11_ctx.vsync, 0);
}

void mg_d3d11_renderer_configure_swapchain(mg_swapchain_config_info *config_info)
{
    ID3D11RenderTargetView_Release(d3d11_ctx.target_view);
    IDXGISwapChain_ResizeBuffers(
        d3d11_ctx.swapchain,
        0,
        config_info->width,
        config_info->height,
        mg_d3d11_get_pixel_format(config_info->format),
        0
    );
    ID3D11Texture2D* pBackBuffer = NULL;
    IDXGISwapChain_GetBuffer(d3d11_ctx.swapchain, 0, &IID_ID3D11Texture2D, (void**)&pBackBuffer);
    ID3D11Device_CreateRenderTargetView(d3d11_ctx.device, (ID3D11Resource*)pBackBuffer, NULL, &d3d11_ctx.target_view);
    ID3D11Buffer_Release(pBackBuffer);

    d3d11_ctx.vsync = config_info->vsync;
}

void mg_d3d11_renderer_viewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    D3D11_VIEWPORT vp = { 0 };
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = (FLOAT)x;
    vp.TopLeftY = (FLOAT)y;
    ID3D11DeviceContext_RSSetViewports(d3d11_ctx.immediate_context, 1, &vp);
}

void mg_d3d11_renderer_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    D3D11_RECT rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;
    ID3D11DeviceContext_RSSetScissorRects(d3d11_ctx.immediate_context, 1, &rect);
}

void mg_d3d11_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    ID3D11DeviceContext_Draw(d3d11_ctx.immediate_context, vertex_count, first_vertex);
}

void mg_d3d11_renderer_draw_indexed(uint32_t index_count, uint32_t first_index, int32_t first_vertex)
{
    ID3D11DeviceContext_DrawIndexed(d3d11_ctx.immediate_context, index_count, first_index, first_vertex);
}

void mg_d3d11_renderer_draw_instanced(uint32_t vertex_count, uint32_t first_vertex, uint32_t instance_count, uint32_t first_instance)
{
    ID3D11DeviceContext_DrawInstanced(d3d11_ctx.immediate_context, vertex_count, instance_count, first_vertex, first_instance);
}

void mg_d3d11_renderer_draw_indexed_instanced(uint32_t index_count, uint32_t first_index, int32_t first_vertex, uint32_t instance_count, uint32_t first_instance)
{
    ID3D11DeviceContext_DrawIndexedInstanced(d3d11_ctx.immediate_context, index_count, instance_count, first_index, first_vertex, first_instance);
}

void mg_d3d11_renderer_dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
{
    ID3D11DeviceContext_Dispatch(d3d11_ctx.immediate_context, group_count_x, group_count_y, group_count_z);
}

void mg_d3d11_renderer_bind_uniforms(uint32_t binding, size_t size, void *data)
{
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    ID3D11DeviceContext_Map(d3d11_ctx.immediate_context, (ID3D11Resource*)d3d11_ctx.constant_buffers[binding], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    memcpy(mapped_resource.pData, data, size);
    ID3D11DeviceContext_Unmap(d3d11_ctx.immediate_context, (ID3D11Resource*)d3d11_ctx.constant_buffers[binding], 0);
    //ID3D11DeviceContext_UpdateSubresource(d3d11_ctx.immediate_context,
        //(ID3D11Resource*)d3d11_ctx.constant_buffers[binding], 0, NULL, data, 0, 0);
}

#endif