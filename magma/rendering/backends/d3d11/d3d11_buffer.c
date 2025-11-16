#include "d3d11_buffer.h"

#if MG_PLATFORM_WINDOWS

#include "d3d11_utils.h"

mg_d3d11_buffer *mg_d3d11_create_buffer(mg_buffer_create_info *create_info)
{
    mg_d3d11_buffer *buffer = (mg_d3d11_buffer*)malloc(sizeof(mg_d3d11_buffer));

    D3D11_BUFFER_DESC desc = { 0 };
    if (create_info->access == MG_ACCESS_TYPE_GPU)
    {
        desc.Usage = D3D11_USAGE_DEFAULT;
        buffer->is_cpu = false;
    }
    else
    {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        buffer->is_cpu = true;
    }

    desc.ByteWidth = (UINT)create_info->size;
    desc.BindFlags = mg_d3d11_get_bind_flags(create_info->usage);

    D3D11_SUBRESOURCE_DATA *init_data_ptr = NULL;
    D3D11_SUBRESOURCE_DATA init_data = { 0 };
    if (create_info->data)
    {
        init_data.pSysMem = create_info->data;
        init_data_ptr = &init_data;
    }

    ID3D11Device_CreateBuffer(d3d11_ctx.device, &desc, init_data_ptr, &buffer->buffer);

    return buffer;
}

void mg_d3d11_update_buffer(mg_d3d11_buffer *buffer, size_t size, void *data)
{
    if (buffer->is_cpu)
    {
        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        ID3D11DeviceContext_Map(d3d11_ctx.immediate_context, (ID3D11Resource*)buffer->buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
        memcpy(mapped_resource.pData, data, size);
        ID3D11DeviceContext_Unmap(d3d11_ctx.immediate_context, (ID3D11Resource*)buffer->buffer, 0);
        return;
    }

    ID3D11DeviceContext_UpdateSubresource(
        d3d11_ctx.immediate_context,
        (ID3D11Resource*)buffer->buffer,
        0,
        NULL,
        data,
        0,
        0
    );
}

void mg_d3d11_bind_vertex_buffer(mg_d3d11_buffer *buffer)
{
    UINT offset = 0;
    ID3D11DeviceContext_IASetVertexBuffers(d3d11_ctx.immediate_context, 0, 1,
        &buffer->buffer, &d3d11_ctx.current_pipeline->layout_stride, &offset);
}

void mg_d3d11_bind_index_buffer(mg_d3d11_buffer *buffer, mg_index_type index_type)
{
    ID3D11DeviceContext_IASetIndexBuffer(d3d11_ctx.immediate_context, buffer->buffer,
        mg_d3d11_get_index_type(index_type), 0);
}

void mg_d3d11_destroy_buffer(mg_d3d11_buffer *buffer)
{
    ID3D11Buffer_Release(buffer->buffer);
    free(buffer);
}

#endif