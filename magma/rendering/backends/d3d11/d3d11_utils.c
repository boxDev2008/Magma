#include "d3d11_utils.h"

#if MG_PLATFORM_WINDOWS

DXGI_FORMAT mg_d3d11_get_pixel_format(mg_pixel_format format)
{
    switch (format)
    {
        case MG_PIXEL_FORMAT_R8_UNORM:
            return DXGI_FORMAT_R8_UNORM;
        case MG_PIXEL_FORMAT_R8_SNORM:
            return DXGI_FORMAT_R8_SNORM;
        case MG_PIXEL_FORMAT_R8_USCALED:
        case MG_PIXEL_FORMAT_R8_SSCALED:
        case MG_PIXEL_FORMAT_R8_UINT:
            return DXGI_FORMAT_R8_UINT;
        case MG_PIXEL_FORMAT_R8_SINT:
            return DXGI_FORMAT_R8_SINT;
        case MG_PIXEL_FORMAT_R8_SRGB:
            return DXGI_FORMAT_R8_TYPELESS;

        case MG_PIXEL_FORMAT_RG8_UNORM:
            return DXGI_FORMAT_R8G8_UNORM;
        case MG_PIXEL_FORMAT_RG8_SNORM:
            return DXGI_FORMAT_R8G8_SNORM;
        case MG_PIXEL_FORMAT_RG8_USCALED:
        case MG_PIXEL_FORMAT_RG8_SSCALED:
        case MG_PIXEL_FORMAT_RG8_UINT:
            return DXGI_FORMAT_R8G8_UINT;
        case MG_PIXEL_FORMAT_RG8_SINT:
            return DXGI_FORMAT_R8G8_SINT;
        case MG_PIXEL_FORMAT_RG8_SRGB:
            return DXGI_FORMAT_R8G8_TYPELESS;

        case MG_PIXEL_FORMAT_RGB8_UNORM:
        case MG_PIXEL_FORMAT_RGB8_SNORM:
        case MG_PIXEL_FORMAT_RGB8_USCALED:
        case MG_PIXEL_FORMAT_RGB8_SSCALED:
        case MG_PIXEL_FORMAT_RGB8_UINT:
        case MG_PIXEL_FORMAT_RGB8_SINT:
        case MG_PIXEL_FORMAT_RGB8_SRGB:
            return DXGI_FORMAT_UNKNOWN; // DXGI doesn't support 24-bit formats directly

        case MG_PIXEL_FORMAT_RGBA8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case MG_PIXEL_FORMAT_RGBA8_SNORM:
            return DXGI_FORMAT_R8G8B8A8_SNORM;
        case MG_PIXEL_FORMAT_RGBA8_USCALED:
        case MG_PIXEL_FORMAT_RGBA8_SSCALED:
        case MG_PIXEL_FORMAT_RGBA8_UINT:
            return DXGI_FORMAT_R8G8B8A8_UINT;
        case MG_PIXEL_FORMAT_RGBA8_SINT:
            return DXGI_FORMAT_R8G8B8A8_SINT;
        case MG_PIXEL_FORMAT_RGBA8_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        case MG_PIXEL_FORMAT_BGRA8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case MG_PIXEL_FORMAT_BGRA8_SNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case MG_PIXEL_FORMAT_BGRA8_USCALED:
        case MG_PIXEL_FORMAT_BGRA8_SSCALED:
        case MG_PIXEL_FORMAT_BGRA8_UINT:
            return DXGI_FORMAT_B8G8R8A8_TYPELESS;
        case MG_PIXEL_FORMAT_BGRA8_SINT:
            return DXGI_FORMAT_B8G8R8A8_TYPELESS;
        case MG_PIXEL_FORMAT_BGRA8_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

        case MG_PIXEL_FORMAT_R16_UNORM:
            return DXGI_FORMAT_R16_UNORM;
        case MG_PIXEL_FORMAT_R16_SNORM:
            return DXGI_FORMAT_R16_SNORM;
        case MG_PIXEL_FORMAT_R16_USCALED:
        case MG_PIXEL_FORMAT_R16_SSCALED:
        case MG_PIXEL_FORMAT_R16_UINT:
            return DXGI_FORMAT_R16_UINT;
        case MG_PIXEL_FORMAT_R16_SINT:
            return DXGI_FORMAT_R16_SINT;
        case MG_PIXEL_FORMAT_R16_SFLOAT:
            return DXGI_FORMAT_R16_FLOAT;

        case MG_PIXEL_FORMAT_RG16_UNORM:
            return DXGI_FORMAT_R16G16_UNORM;
        case MG_PIXEL_FORMAT_RG16_SNORM:
            return DXGI_FORMAT_R16G16_SNORM;
        case MG_PIXEL_FORMAT_RG16_USCALED:
        case MG_PIXEL_FORMAT_RG16_SSCALED:
        case MG_PIXEL_FORMAT_RG16_UINT:
            return DXGI_FORMAT_R16G16_UINT;
        case MG_PIXEL_FORMAT_RG16_SINT:
            return DXGI_FORMAT_R16G16_SINT;
        case MG_PIXEL_FORMAT_RG16_SFLOAT:
            return DXGI_FORMAT_R16G16_FLOAT;

        case MG_PIXEL_FORMAT_RGB16_UNORM:
        case MG_PIXEL_FORMAT_RGB16_SNORM:
        case MG_PIXEL_FORMAT_RGB16_USCALED:
        case MG_PIXEL_FORMAT_RGB16_SSCALED:
        case MG_PIXEL_FORMAT_RGB16_UINT:
        case MG_PIXEL_FORMAT_RGB16_SINT:
        case MG_PIXEL_FORMAT_RGB16_SFLOAT:
            return DXGI_FORMAT_UNKNOWN; // No native support for these formats

        case MG_PIXEL_FORMAT_RGBA16_UNORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case MG_PIXEL_FORMAT_RGBA16_SNORM:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
        case MG_PIXEL_FORMAT_RGBA16_USCALED:
        case MG_PIXEL_FORMAT_RGBA16_SSCALED:
        case MG_PIXEL_FORMAT_RGBA16_UINT:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case MG_PIXEL_FORMAT_RGBA16_SINT:
            return DXGI_FORMAT_R16G16B16A16_SINT;
        case MG_PIXEL_FORMAT_RGBA16_SFLOAT:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case MG_PIXEL_FORMAT_R32_UINT:
            return DXGI_FORMAT_R32_UINT;
        case MG_PIXEL_FORMAT_R32_SINT:
            return DXGI_FORMAT_R32_SINT;
        case MG_PIXEL_FORMAT_R32_SFLOAT:
            return DXGI_FORMAT_R32_FLOAT;

        case MG_PIXEL_FORMAT_RG32_UINT:
            return DXGI_FORMAT_R32G32_UINT;
        case MG_PIXEL_FORMAT_RG32_SINT:
            return DXGI_FORMAT_R32G32_SINT;
        case MG_PIXEL_FORMAT_RG32_SFLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;

        case MG_PIXEL_FORMAT_RGB32_UINT:
        case MG_PIXEL_FORMAT_RGB32_SINT:
        case MG_PIXEL_FORMAT_RGB32_SFLOAT:
            return DXGI_FORMAT_UNKNOWN; // No native support

        case MG_PIXEL_FORMAT_RGBA32_UINT:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case MG_PIXEL_FORMAT_RGBA32_SINT:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case MG_PIXEL_FORMAT_RGBA32_SFLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case MG_PIXEL_FORMAT_RGB64_UINT:
        case MG_PIXEL_FORMAT_RGB64_SINT:
        case MG_PIXEL_FORMAT_RGB64_SFLOAT:
            return DXGI_FORMAT_UNKNOWN;

        case MG_PIXEL_FORMAT_D32_SFLOAT:
            return DXGI_FORMAT_R32_TYPELESS;
        case MG_PIXEL_FORMAT_D16_UNORM_S8_UINT:
            return DXGI_FORMAT_R16_TYPELESS;
        case MG_PIXEL_FORMAT_D24_UNORM_S8_UINT:
            return DXGI_FORMAT_R24G8_TYPELESS;
        case MG_PIXEL_FORMAT_D32_SFLOAT_S8_UINT:
            return DXGI_FORMAT_R32G8X24_TYPELESS;

        default:
            return DXGI_FORMAT_UNKNOWN;
    }
}

DXGI_FORMAT mg_d3d11_get_vertex_format(mg_vertex_format format)
{
    switch (format)
    {
        case MG_VERTEX_FORMAT_FLOAT: return DXGI_FORMAT_R32_FLOAT; break;
        case MG_VERTEX_FORMAT_FLOAT2: return DXGI_FORMAT_R32G32_FLOAT; break;
        case MG_VERTEX_FORMAT_FLOAT3: return DXGI_FORMAT_R32G32B32_FLOAT; break;
        case MG_VERTEX_FORMAT_FLOAT4: return DXGI_FORMAT_R32G32B32A32_FLOAT; break;

        case MG_VERTEX_FORMAT_UINT: return DXGI_FORMAT_R32_UINT; break;
        case MG_VERTEX_FORMAT_UINT2: return DXGI_FORMAT_R32G32_UINT; break;
        case MG_VERTEX_FORMAT_UINT3: return DXGI_FORMAT_R32G32B32_UINT; break;
        case MG_VERTEX_FORMAT_UINT4: return DXGI_FORMAT_R32G32B32A32_UINT; break;

        case MG_VERTEX_FORMAT_INT: return DXGI_FORMAT_R32_SINT; break;
        case MG_VERTEX_FORMAT_INT2: return DXGI_FORMAT_R32G32_SINT; break;
        case MG_VERTEX_FORMAT_INT3: return DXGI_FORMAT_R32G32B32_SINT; break;
        case MG_VERTEX_FORMAT_INT4: return DXGI_FORMAT_R32G32B32A32_SINT; break;

        case MG_VERTEX_FORMAT_UBYTE4: return DXGI_FORMAT_R8G8B8A8_UINT; break;
        case MG_VERTEX_FORMAT_BYTE4: return DXGI_FORMAT_R8G8B8A8_SINT; break;

        case MG_VERTEX_FORMAT_UBYTE4N: return DXGI_FORMAT_R8G8B8A8_UNORM; break;
        case MG_VERTEX_FORMAT_BYTE4N: return DXGI_FORMAT_R8G8B8A8_SNORM; break;

        default: return DXGI_FORMAT_UNKNOWN;
    }
}

DXGI_FORMAT mg_d3d11_get_index_type(mg_index_type index_type)
{
    if (index_type == MG_INDEX_TYPE_UINT16)
        return DXGI_FORMAT_R16_UINT;
    return DXGI_FORMAT_R32_UINT;
}

D3D11_CULL_MODE mg_d3d11_get_cull_mode(mg_cull_mode mode)
{
    switch (mode)
    {
        case MG_CULL_MODE_NONE: return D3D11_CULL_NONE;
        case MG_CULL_MODE_FRONT: return D3D11_CULL_FRONT;
        case MG_CULL_MODE_BACK: return D3D11_CULL_BACK;
        case MG_CULL_MODE_FRONT_AND_BACK: return D3D11_CULL_NONE;
    }
    return D3D11_CULL_NONE;
}

D3D11_COMPARISON_FUNC mg_d3d11_get_comparison_func(mg_compare_op func)
{
    switch (func)
    {
        case MG_COMPARE_OP_NEVER: return D3D11_COMPARISON_NEVER;
        case MG_COMPARE_OP_LESS: return D3D11_COMPARISON_LESS;
        case MG_COMPARE_OP_EQUAL: return D3D11_COMPARISON_EQUAL;
        case MG_COMPARE_OP_LESS_OR_EQUAL: return D3D11_COMPARISON_LESS_EQUAL;
        case MG_COMPARE_OP_GREATER: return D3D11_COMPARISON_GREATER;
        case MG_COMPARE_OP_NOT_EQUAL: return D3D11_COMPARISON_NOT_EQUAL;
        case MG_COMPARE_OP_GREATER_OR_EQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
        case MG_COMPARE_OP_ALWAYS: return D3D11_COMPARISON_ALWAYS;
    }
    return D3D11_COMPARISON_NEVER;
}

DXGI_FORMAT mg_d3d11_get_dsv_format(mg_pixel_format format)
{
    switch (format)
    {
        case MG_PIXEL_FORMAT_D16_UNORM_S8_UINT:
            return DXGI_FORMAT_D16_UNORM;
        case MG_PIXEL_FORMAT_D24_UNORM_S8_UINT:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case MG_PIXEL_FORMAT_D32_SFLOAT:
            return DXGI_FORMAT_D32_FLOAT;
        case MG_PIXEL_FORMAT_D32_SFLOAT_S8_UINT:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    }
    return DXGI_FORMAT_UNKNOWN;
}

DXGI_FORMAT mg_d3d11_get_srv_format(mg_pixel_format format)
{
    switch (format)
    {
        case MG_PIXEL_FORMAT_D16_UNORM_S8_UINT:
            return DXGI_FORMAT_R16_UNORM;
        case MG_PIXEL_FORMAT_D24_UNORM_S8_UINT:
            return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case MG_PIXEL_FORMAT_D32_SFLOAT:
            return DXGI_FORMAT_R32_FLOAT;
        case MG_PIXEL_FORMAT_D32_SFLOAT_S8_UINT:
            return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    }
    return mg_d3d11_get_pixel_format(format);
}

D3D_SRV_DIMENSION mg_d3d11_get_srv_dimension(mg_image_type type)
{
    switch (type)
    {
        case MG_IMAGE_TYPE_2D: return D3D11_SRV_DIMENSION_TEXTURE2D;
        case MG_IMAGE_TYPE_3D: return D3D11_SRV_DIMENSION_TEXTURE3D;
        case MG_IMAGE_TYPE_CUBE: return D3D11_SRV_DIMENSION_TEXTURECUBE;
    }
    return D3D_SRV_DIMENSION_TEXTURE2D;
}

D3D11_FILTER mg_d3d11_get_filter(mg_sampler_filter filter)
{
    switch (filter)
    {
        case MG_SAMPLER_FILTER_NEAREST: return D3D11_FILTER_MIN_MAG_MIP_POINT;
        case MG_SAMPLER_FILTER_LINEAR: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    }
    return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

D3D11_TEXTURE_ADDRESS_MODE mg_d3d11_get_texture_address_mode(mg_sampler_address_mode mode)
{
    switch (mode)
    {
        case MG_SAMPLER_ADDRESS_MODE_REPEAT: return D3D11_TEXTURE_ADDRESS_WRAP;
        case MG_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: return D3D11_TEXTURE_ADDRESS_MIRROR;
        case MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: return D3D11_TEXTURE_ADDRESS_CLAMP;
        case MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: return D3D11_TEXTURE_ADDRESS_BORDER;
        case MG_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
    }
    return D3D11_TEXTURE_ADDRESS_WRAP;
}

D3D11_BLEND mg_d3d11_get_blend_factor(mg_blend_factor factor)
{
    switch (factor)
    {
        case MG_BLEND_FACTOR_ZERO: return D3D11_BLEND_ZERO;
        case MG_BLEND_FACTOR_ONE: return D3D11_BLEND_ONE;
        case MG_BLEND_FACTOR_SRC_COLOR: return D3D11_BLEND_SRC_COLOR;
        case MG_BLEND_FACTOR_ONE_MINUS_SRC_COLOR: return D3D11_BLEND_INV_SRC_COLOR;
        case MG_BLEND_FACTOR_DST_COLOR: return D3D11_BLEND_DEST_COLOR;
        case MG_BLEND_FACTOR_ONE_MINUS_DST_COLOR: return D3D11_BLEND_INV_DEST_COLOR;
        case MG_BLEND_FACTOR_SRC_ALPHA: return D3D11_BLEND_SRC_ALPHA;
        case MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA: return D3D11_BLEND_INV_SRC_ALPHA;
        case MG_BLEND_FACTOR_DST_ALPHA: return D3D11_BLEND_DEST_ALPHA;
        case MG_BLEND_FACTOR_ONE_MINUS_DST_ALPHA: return D3D11_BLEND_INV_DEST_ALPHA;
        /*case MG_BLEND_FACTOR_CONSTANT_COLOR: return D3D11_BLEND_CONSTANT;
        case MG_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR: return D3D11_BLEND_INV_CONST_COLOR;
        case MG_BLEND_FACTOR_CONSTANT_ALPHA: return D3D11_BLEND_CONSTANT_ALPHA;
        case MG_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA: return D3D11_BLEND_INV_CONST_ALPHA;*/
        case MG_BLEND_FACTOR_SRC_ALPHA_SATURATE: return D3D11_BLEND_SRC_ALPHA_SAT;
        case MG_BLEND_FACTOR_SRC1_COLOR: return D3D11_BLEND_SRC1_COLOR;
        case MG_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR: return D3D11_BLEND_INV_SRC1_COLOR;
        case MG_BLEND_FACTOR_SRC1_ALPHA: return D3D11_BLEND_SRC1_ALPHA;
        case MG_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA: return D3D11_BLEND_INV_SRC1_ALPHA;
    }
    return D3D11_BLEND_ZERO;
}

D3D11_BLEND_OP mg_d3d11_get_blend_op(mg_blend_op op)
{
    switch (op)
    {
        case MG_BLEND_OP_ADD: return D3D11_BLEND_OP_ADD;
        case MG_BLEND_OP_SUBTRACT: return D3D11_BLEND_OP_SUBTRACT;
        case MG_BLEND_OP_REVERSE_SUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
        case MG_BLEND_OP_MIN: return D3D11_BLEND_OP_MIN;
        case MG_BLEND_OP_MAX: return D3D11_BLEND_OP_MAX;
    }
    return D3D11_BLEND_OP_ADD;
}

D3D11_PRIMITIVE_TOPOLOGY mg_d3d11_get_primitive_topology(mg_primitive_topology topology)
{
    switch (topology)
    {
        case MG_PRIMITIVE_TOPOLOGY_POINT_LIST: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case MG_PRIMITIVE_TOPOLOGY_LINE_LIST: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case MG_PRIMITIVE_TOPOLOGY_LINE_STRIP: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case MG_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
        case MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case MG_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    }
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

D3D11_FILL_MODE mg_d3d11_get_fill_mode(mg_polygon_mode mode)
{
    switch (mode)
    {
        case MG_POLYGON_MODE_FILL: return D3D11_FILL_SOLID;
        case MG_POLYGON_MODE_POINT:
        case MG_POLYGON_MODE_LINE: return D3D11_FILL_WIREFRAME;
    }
    return D3D11_FILL_SOLID;
}

UINT mg_d3d11_get_bind_flags(mg_buffer_usage usage)
{
    switch (usage)
    {
        case MG_BUFFER_USAGE_VERTEX: return D3D11_BIND_VERTEX_BUFFER;
        case MG_BUFFER_USAGE_INDEX: return D3D11_BIND_INDEX_BUFFER;
    }
    return D3D11_BIND_VERTEX_BUFFER;
}

#endif