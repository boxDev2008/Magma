#include "d3d11_render_pass.h"

#if MG_PLATFORM_WINDOWS

#include "d3d11_renderer.h"
#include "d3d11_utils.h"

mg_d3d11_render_pass *mg_d3d11_create_render_pass(const mg_render_pass_create_info *create_info)
{
    mg_d3d11_render_pass *render_pass = (mg_d3d11_render_pass*)malloc(sizeof(mg_d3d11_render_pass));

    if (create_info->color_attachment.image)
    {
        mg_d3d11_image *color_attachment = (mg_d3d11_image*)create_info->color_attachment.image;
        ID3D11Device_CreateRenderTargetView(d3d11_ctx.device, color_attachment->texture, NULL, &render_pass->color_attachment);
    }
    else render_pass->color_attachment = NULL;

    if (create_info->depth_stencil_attachment.image)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = { 0 };
        dsv_desc.Format = mg_d3d11_get_dsv_format(create_info->depth_stencil_attachment.format);
        dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        mg_d3d11_image *depth_stencil_attachment = (mg_d3d11_image*)create_info->depth_stencil_attachment.image;
        ID3D11Device_CreateDepthStencilView(d3d11_ctx.device, depth_stencil_attachment->texture, &dsv_desc, &render_pass->depth_stencil_attachment);
        render_pass->dsv_format = dsv_desc.Format;
    }
    else render_pass->depth_stencil_attachment = NULL;

    return render_pass;
}

void mg_d3d11_destroy_render_pass(mg_d3d11_render_pass *render_pass)
{
    if (render_pass->color_attachment)
        ID3D11RenderTargetView_Release(render_pass->color_attachment);
	if (render_pass->depth_stencil_attachment)
    	ID3D11DepthStencilView_Release(render_pass->depth_stencil_attachment);
    free(render_pass);
}

void mg_d3d11_update_render_pass(mg_d3d11_render_pass *render_pass, const mg_render_pass_update_info *update_info)
{
    mg_d3d11_image *color_attachment = (mg_d3d11_image*)update_info->color_image;
    
    if (update_info->color_image)
    {
        if (render_pass->color_attachment)
            ID3D11RenderTargetView_Release(render_pass->color_attachment);

        mg_d3d11_image *color_attachment = (mg_d3d11_image*)update_info->color_image;
        ID3D11Device_CreateRenderTargetView(d3d11_ctx.device, color_attachment->texture, NULL, &render_pass->color_attachment);
    }

    if (update_info->depth_stencil_image)
    {
        if (render_pass->depth_stencil_attachment)
            ID3D11DepthStencilView_Release(render_pass->depth_stencil_attachment);

        D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = { 0 };
        dsv_desc.Format = render_pass->dsv_format;
        dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        mg_d3d11_image *depth_stencil_attachment = (mg_d3d11_image*)update_info->depth_stencil_image;
        ID3D11Device_CreateDepthStencilView(d3d11_ctx.device, depth_stencil_attachment->texture, &dsv_desc, &render_pass->depth_stencil_attachment);
    }
}

static void mg_d3d11_begin_clear_render_target(ID3D11RenderTargetView *color_attachment, const mg_render_pass_bind_info *begin_info)
{
    mg_d3d11_renderer_viewport(
        begin_info->render_area.x,
        begin_info->render_area.y,
        begin_info->render_area.z,
        begin_info->render_area.w
    );

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

    ID3D11DeviceContext_ClearRenderTargetView(d3d11_ctx.immediate_context, color_attachment, clear_color);
}

void mg_d3d11_bind_render_pass(mg_d3d11_render_pass *render_pass, const mg_render_pass_bind_info *bind_info)
{
    if (!render_pass)
    {
        ID3D11DeviceContext_OMSetRenderTargets(d3d11_ctx.immediate_context, 1, &d3d11_ctx.target_view, NULL);
        mg_d3d11_begin_clear_render_target(d3d11_ctx.target_view, bind_info);
        return;
    }

    ID3D11DeviceContext_OMSetRenderTargets(
        d3d11_ctx.immediate_context,
        1,
        &render_pass->color_attachment,
        render_pass->depth_stencil_attachment
    );

    mg_d3d11_begin_clear_render_target(render_pass->color_attachment, bind_info);

	if (render_pass->depth_stencil_attachment)
    {
		ID3D11DeviceContext_ClearDepthStencilView(d3d11_ctx.immediate_context, render_pass->depth_stencil_attachment,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

#endif