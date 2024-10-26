#include "d3d11_buffer.h"

#if MG_PLATFORM_WINDOWS

#include "d3d11_utils.h"

ID3D11Buffer *mg_d3d11_create_buffer(size_t size, UINT bind_flags, void *data)
{
    ID3D11Buffer *buffer = NULL;

    D3D11_BUFFER_DESC desc = { 0 };
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = size;
    desc.BindFlags = bind_flags;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA init_data = { 0 };
    init_data.pSysMem = data;

    ID3D11Device_CreateBuffer(d3d11_context.device, &desc, &init_data, &buffer);

    return buffer;
}

ID3D11Buffer *mg_d3d11_create_dynamic_buffer(size_t size, UINT bind_flags)
{
    ID3D11Buffer *buffer = NULL;

    D3D11_BUFFER_DESC desc = { 0 };
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = size;
    desc.BindFlags = bind_flags;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ID3D11Device_CreateBuffer(d3d11_context.device, &desc, 0, &buffer);

    return buffer;
}

ID3D11Buffer *mg_d3d11_create_vertex_buffer(size_t size, void *data)
{
    return mg_d3d11_create_buffer(size, D3D11_BIND_VERTEX_BUFFER, data);
}

ID3D11Buffer *mg_d3d11_create_index_buffer(size_t size, void *data)
{
    return mg_d3d11_create_buffer(size, D3D11_BIND_INDEX_BUFFER, data);
}

ID3D11Buffer *mg_d3d11_create_dynamic_vertex_buffer(size_t size)
{
    return mg_d3d11_create_dynamic_buffer(size, D3D11_BIND_VERTEX_BUFFER);
}

ID3D11Buffer *mg_d3d11_create_dynamic_index_buffer(size_t size)
{
    return mg_d3d11_create_dynamic_buffer(size, D3D11_BIND_INDEX_BUFFER);
}

void mg_d3d11_update_dynamic_buffer(ID3D11Buffer *buffer, size_t size, void *data)
{
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    ID3D11DeviceContext_Map(d3d11_context.immediate_context, buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
    memcpy(mapped_resource.pData, data, size);
    ID3D11DeviceContext_Unmap(d3d11_context.immediate_context, buffer, 0);
}

ID3D11Buffer *mg_d3d11_create_uniform_buffer(size_t size)
{
    ID3D11Buffer *buffer = NULL;
    D3D11_BUFFER_DESC desc = { 0 };

    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = size;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;

    ID3D11Device_CreateBuffer(d3d11_context.device, &desc, NULL, &buffer);

    return buffer;
}

void mg_d3d11_bind_vertex_buffer(ID3D11Buffer *buffer)
{
    UINT offset = 0;
    ID3D11DeviceContext_IASetVertexBuffers(d3d11_context.immediate_context, 0, 1,
        &buffer, &d3d11_context.binds.pipeline->layout_stride, &offset);
}

void mg_d3d11_bind_index_buffer(ID3D11Buffer *buffer, mg_index_type index_type)
{
    ID3D11DeviceContext_IASetIndexBuffer(d3d11_context.immediate_context, buffer,
        mg_d3d11_get_index_type(index_type), 0);
}

void mg_d3d11_update_uniform_buffer(ID3D11Buffer *buffer, size_t size, void *data)
{
    ID3D11DeviceContext_UpdateSubresource(d3d11_context.immediate_context, buffer, 0, NULL, data, 0, 0);
}

void mg_d3d11_bind_uniform_buffer(ID3D11Buffer *buffer)
{
    ID3D11DeviceContext_VSSetConstantBuffers(d3d11_context.immediate_context, 0, 1, &buffer);
}

void mg_d3d11_destroy_buffer(ID3D11Buffer *buffer)
{
    ID3D11Buffer_Release(buffer);
}

#endif