#pragma once

#include "opengl_types.inl"
#include "../llapi_renderer.h"

mg_opengl_pipeline_t   *mg_opengl_create_pipeline   (mg_pipeline_create_info_t *create_info);
void                    mg_opengl_destroy_pipeline  (mg_opengl_pipeline_t *pipeline);
void                    mg_opengl_bind_pipeline     (mg_opengl_pipeline_t *pipeline);