#pragma once

#include "d3d11_types.h"

#if MG_PLATFORM_WINDOWS

mg_d3d11_image *mg_d3d11_create_image(mg_image_create_info *create_info);
void mg_d3d11_destroy_image(mg_d3d11_image *image);
void mg_d3d11_update_image(mg_d3d11_image *image, mg_image_update_info *write_info);
void mg_d3d11_bind_image(mg_d3d11_image *image, ID3D11SamplerState *sampler, uint32_t binding);

ID3D11SamplerState *mg_d3d11_create_sampler(mg_sampler_create_info *create_info);
void mg_d3d11_destroy_sampler(ID3D11SamplerState *sampler);

#endif