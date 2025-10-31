#pragma once

#include "opengl_types.h"

mg_opengl_image		   *mg_opengl_create_image			(mg_image_create_info *create_info);
void						mg_opengl_destroy_image			(mg_opengl_image *image);
void						mg_opengl_update_image			(mg_opengl_image *image, mg_image_write_info *write_info);

mg_opengl_image_array    *mg_opengl_create_image_array  (void);
void                            mg_opengl_destroy_image_array (mg_opengl_image_array *array);
void                            mg_opengl_update_image_array  (mg_opengl_image_array *array, mg_opengl_image **images, mg_opengl_sampler **samplers, uint32_t count);
void                            mg_opengl_bind_image_array    (mg_opengl_image_array *array);

mg_opengl_sampler		   *mg_opengl_create_sampler		(mg_sampler_create_info *create_info);
void						mg_opengl_destroy_sampler		(mg_opengl_sampler *sampler);