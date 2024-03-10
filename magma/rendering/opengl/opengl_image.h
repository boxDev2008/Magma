#pragma once

#include "opengl_types.h"

mg_opengl_image_t		   *mg_opengl_create_image			(mg_image_create_info_t *create_info);
void						mg_opengl_destroy_image			(mg_opengl_image_t *image);
void						mg_opengl_write_image			(mg_opengl_image_t *image, mg_image_write_info_t *write_info);
void						mg_opengl_update_image			(mg_opengl_image_t *image, mg_opengl_sampler_t *sampler, uint32_t binding);
void						mg_opengl_bind_image			(mg_opengl_image_t *image, mg_opengl_pipeline_t *pipeline);

mg_opengl_sampler_t		   *mg_opengl_create_sampler		(mg_sampler_create_info_t *create_info);
void						mg_opengl_destroy_sampler		(mg_opengl_sampler_t *sampler);

mg_opengl_framebuffer_t	   *mg_opengl_create_framebuffer	(mg_framebuffer_create_info_t *create_info);
void						mg_opengl_destroy_framebuffer	(mg_opengl_framebuffer_t *framebuffer);