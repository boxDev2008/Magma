#include "d3d11_render_pass.h"

#if MG_PLATFORM_WINDOWS

#include "d3d11_renderer.h"

mg_d3d11_render_pass *mg_d3d11_create_render_pass(mg_render_pass_create_info *create_info)
{
    return NULL;
}

void mg_d3d11_destroy_render_pass(mg_d3d11_render_pass *render_pass)
{

}

void mg_d3d11_begin_render_pass(mg_d3d11_render_pass *render_pass, mg_d3d11_framebuffer *framebuffer, mg_render_pass_begin_info *begin_info)
{
    ID3D11DeviceContext_OMSetRenderTargets(d3d11_ctx.immediate_context, 1, &framebuffer->color_attachment, framebuffer->depth_stencil_attachment);
    mg_d3d11_renderer_scissor(
        begin_info->render_area.x,
        begin_info->render_area.y,
        begin_info->render_area.z,
        begin_info->render_area.w
    );

    const float clear_color[4] = {
        begin_info->clear_value.r,
        begin_info->clear_value.g,
        begin_info->clear_value.b,
        begin_info->clear_value.a
    };

    ID3D11DeviceContext_ClearRenderTargetView(d3d11_ctx.immediate_context, framebuffer->color_attachment, clear_color);

	if (framebuffer->depth_stencil_attachment)
    {
		ID3D11DeviceContext_ClearDepthStencilView(d3d11_ctx.immediate_context, framebuffer->depth_stencil_attachment,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

void mg_d3d11_begin_default_render_pass(mg_render_pass_begin_info *begin_info)
{
    ID3D11DeviceContext_OMSetRenderTargets(d3d11_ctx.immediate_context, 1, &d3d11_ctx.target_view, NULL);
    mg_d3d11_renderer_scissor(
        begin_info->render_area.x,
        begin_info->render_area.y,
        begin_info->render_area.z,
        begin_info->render_area.w
    );

    const float clear_color[4] = {
        begin_info->clear_value.r,
        begin_info->clear_value.g,
        begin_info->clear_value.b,
        begin_info->clear_value.a
    };

    ID3D11DeviceContext_ClearRenderTargetView(d3d11_ctx.immediate_context, d3d11_ctx.target_view, clear_color);
}

void mg_d3d11_end_render_pass(void)
{
	ID3D11DeviceContext_OMSetRenderTargets(d3d11_ctx.immediate_context, 0, NULL, NULL);
}

#endif