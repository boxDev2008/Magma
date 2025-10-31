#pragma once

#include "d3d11_types.h"

#if MG_PLATFORM_WINDOWS

mg_d3d11_image *mg_d3d11_create_image(mg_image_create_info *create_info);
void mg_d3d11_destroy_image(mg_d3d11_image *image);
void mg_d3d11_update_image(mg_d3d11_image *image, mg_image_write_info *write_info);

ID3D11SamplerState *mg_d3d11_create_sampler(mg_sampler_create_info *create_info);
void mg_d3d11_destroy_sampler(ID3D11SamplerState *sampler);

mg_d3d11_image_array *mg_d3d11_create_image_array(void);
void mg_d3d11_destroy_image_array(mg_d3d11_image_array *array);
void mg_d3d11_update_image_array(mg_d3d11_image_array *array, mg_d3d11_image **images, ID3D11SamplerState **samplers, uint32_t count);
void mg_d3d11_bind_image_array(mg_d3d11_image_array *array);
#endif