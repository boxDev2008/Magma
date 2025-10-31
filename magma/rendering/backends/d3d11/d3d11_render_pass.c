#include "d3d11_render_pass.h"

#if MG_PLATFORM_WINDOWS

#include "d3d11_renderer.h"

mg_d3d11_render_pass *mg_d3d11_create_render_pass(mg_render_pass_create_info *create_info)
{
    mg_d3d11_render_pass *render_pass = (mg_d3d11_render_pass*)malloc(sizeof(mg_d3d11_render_pass));

    mg_d3d11_image *color_attachment = (mg_d3d11_image*)create_info->color_attachment.image;
    ID3D11Device_CreateRenderTargetView(d3d11_ctx.device, (ID3D11Resource*)color_attachment->texture, NULL, &render_pass->color_attachment);

    if (create_info->depth_stencil_attachment.image)
    {
        mg_d3d11_image *depth_stencil_attachment = (mg_d3d11_image*)create_info->depth_stencil_attachment.image;
        ID3D11Device_CreateDepthStencilView(d3d11_ctx.device, (ID3D11Resource*)depth_stencil_attachment->texture, NULL, &render_pass->depth_stencil_attachment);
    }
    else
        render_pass->depth_stencil_attachment = NULL;

    return render_pass;
}

void mg_d3d11_destroy_render_pass(mg_d3d11_render_pass *render_pass)
{
    ID3D11RenderTargetView_Release(render_pass->color_attachment);
	if (render_pass->depth_stencil_attachment)
    	ID3D11DepthStencilView_Release(render_pass->depth_stencil_attachment);
    free(render_pass);
}

void mg_d3d11_update_render_pass(mg_d3d11_render_pass *render_pass, mg_render_pass_update_info *resize_info)
{
    mg_d3d11_image *color_attachment = (mg_d3d11_image*)resize_info->color_image;
    ID3D11RenderTargetView_Release(render_pass->color_attachment);
    ID3D11Device_CreateRenderTargetView(d3d11_ctx.device, (ID3D11Resource*)color_attachment->texture, NULL, &render_pass->color_attachment);

    if (resize_info->depth_stencil_image)
    {
        mg_d3d11_image *depth_stencil_attachment = (mg_d3d11_image*)resize_info->depth_stencil_image;
    	ID3D11DepthStencilView_Release(render_pass->depth_stencil_attachment);
        ID3D11Device_CreateDepthStencilView(d3d11_ctx.device, (ID3D11Resource*)depth_stencil_attachment->texture, NULL, &render_pass->depth_stencil_attachment);
    }
    else
        render_pass->depth_stencil_attachment = NULL;
}

void mg_d3d11_begin_render_pass(mg_d3d11_render_pass *render_pass, mg_render_pass_begin_info *begin_info)
{
    ID3D11DeviceContext_OMSetRenderTargets(d3d11_ctx.immediate_context, 1, &render_pass->color_attachment, render_pass->depth_stencil_attachment);
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

    ID3D11DeviceContext_ClearRenderTargetView(d3d11_ctx.immediate_context, render_pass->color_attachment, clear_color);

	if (render_pass->depth_stencil_attachment)
    {
		ID3D11DeviceContext_ClearDepthStencilView(d3d11_ctx.immediate_context, render_pass->depth_stencil_attachment,
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