#pragma once

#include "opengl_types.h"

mg_opengl_image		   *mg_opengl_create_image			(mg_image_create_info *create_info);
void						mg_opengl_destroy_image			(mg_opengl_image *image);
void						mg_opengl_update_image			(mg_opengl_image *image, mg_image_update_info *write_info);
void 					    mg_opengl_bind_image			(mg_opengl_image *image, mg_opengl_sampler *sampler, uint32_t binding);

mg_opengl_sampler		   *mg_opengl_create_sampler		(mg_sampler_create_info *create_info);
void						mg_opengl_destroy_sampler		(mg_opengl_sampler *sampler);