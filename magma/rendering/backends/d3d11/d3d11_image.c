#include "d3d11_image.h"

#if MG_PLATFORM_WINDOWS

#include "d3d11_renderer.h"
#include "d3d11_utils.h"

mg_d3d11_image *mg_d3d11_create_image(mg_image_create_info *create_info)
{
    mg_d3d11_image *image = (mg_d3d11_image*)malloc(sizeof(mg_d3d11_image));

    D3D11_TEXTURE2D_DESC texture_desc = { 0 };
    texture_desc.Width = create_info->width;
    texture_desc.Height = create_info->height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = mg_d3d11_get_pixel_format(create_info->format);
    texture_desc.SampleDesc.Count = 1;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;

    switch (create_info->usage)
    {
    case MG_IMAGE_USAGE_COLOR_ATTACHMENT:
        texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        break;
    case MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT:
        texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        break;
    default:
        texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        break;
    }

    ID3D11Device_CreateTexture2D(d3d11_ctx.device, &texture_desc, NULL, &image->texture);

    if (create_info->usage != MG_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = { 0 };
        view_desc.Format = texture_desc.Format;
        view_desc.ViewDimension = mg_d3d11_get_srv_dimension(create_info->type);
        view_desc.Texture2D.MipLevels = 1;
        ID3D11Device_CreateShaderResourceView(d3d11_ctx.device, (ID3D11Resource*)image->texture, &view_desc, &image->view);
    }
    else image->view = NULL;

    return image;
}

void mg_d3d11_destroy_image(mg_d3d11_image *image)
{
    if (image->view)
        ID3D11ShaderResourceView_Release(image->view);
    ID3D11Texture2D_Release(image->texture);
    free(image);
}

void mg_d3d11_update_image(mg_d3d11_image *image, mg_image_write_info *write_info)
{
    const uint32_t row_pitch = write_info->width * 4;
    ID3D11DeviceContext_UpdateSubresource(
        d3d11_ctx.immediate_context,
        (ID3D11Resource*)image->texture,
        0,
        NULL,
        write_info->data,
        row_pitch,
        0
    );
}

ID3D11SamplerState *mg_d3d11_create_sampler(mg_sampler_create_info *create_info)
{
    ID3D11SamplerState *sampler;
    D3D11_SAMPLER_DESC samp_desc = { 0 };
    samp_desc.Filter = mg_d3d11_get_filter(create_info->min_filter);
    samp_desc.AddressU = mg_d3d11_get_texture_address_mode(create_info->address_mode_u);
    samp_desc.AddressV = mg_d3d11_get_texture_address_mode(create_info->address_mode_v);
    samp_desc.AddressW = mg_d3d11_get_texture_address_mode(create_info->address_mode_w);
    samp_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samp_desc.MinLOD = 0;
    samp_desc.MaxLOD = D3D11_FLOAT32_MAX;

    ID3D11Device_CreateSamplerState(d3d11_ctx.device, &samp_desc, &sampler);
    return sampler;
}

void mg_d3d11_destroy_sampler(ID3D11SamplerState *sampler)
{
    ID3D11SamplerState_Release(sampler);
}

mg_d3d11_image_array *mg_d3d11_create_image_array(void)
{
    return (mg_d3d11_image_array*)malloc(sizeof(mg_d3d11_image_array));
}

void mg_d3d11_destroy_image_array(mg_d3d11_image_array *array)
{
    free(array);
}

void mg_d3d11_update_image_array(mg_d3d11_image_array *array, mg_d3d11_image **images, ID3D11SamplerState **samplers, uint32_t count)
{
    array->count = count;
    for (uint32_t i = 0; i < count; i++)
    {
        array->views[i] = images[i]->view;
        array->samplers[i] = samplers[i];
    }
}

void mg_d3d11_bind_image_array(mg_d3d11_image_array *array)
{
    ID3D11DeviceContext_PSSetShaderResources(d3d11_ctx.immediate_context, 0, array->count, array->views);
    ID3D11DeviceContext_PSSetSamplers(d3d11_ctx.immediate_context, 0, array->count, array->samplers);
}

#endif