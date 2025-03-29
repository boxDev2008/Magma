#pragma once
#include "d3d11_types.h"

#if MG_PLATFORM_WINDOWS

DXGI_FORMAT mg_d3d11_get_pixel_format(mg_pixel_format format);
DXGI_FORMAT mg_d3d11_get_vertex_format(mg_vertex_format format);
DXGI_FORMAT mg_d3d11_get_index_type(mg_index_type index_type);
D3D11_CULL_MODE mg_d3d11_get_cull_mode(mg_cull_mode mode);
D3D11_COMPARISON_FUNC mg_d3d11_get_comparison_func(mg_compare_op func);
D3D_SRV_DIMENSION mg_d3d11_get_srv_dimension(mg_image_type type);
D3D11_FILTER mg_d3d11_get_filter(mg_sampler_filter filter);
D3D11_TEXTURE_ADDRESS_MODE mg_d3d11_get_texture_address_mode(mg_sampler_address_mode mode);
D3D11_BLEND mg_d3d11_get_blend_factor(mg_blend_factor factor);
D3D11_BLEND_OP mg_d3d11_get_blend_op(mg_blend_op op);
D3D11_PRIMITIVE_TOPOLOGY mg_d3d11_get_primitive_topology(mg_primitive_topology topology);
D3D11_FILL_MODE mg_d3d11_get_fill_mode(mg_polygon_mode mode);

#endif