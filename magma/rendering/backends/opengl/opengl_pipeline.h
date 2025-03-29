#pragma once

#include "opengl_types.h"

mg_opengl_pipeline   *mg_opengl_create_pipeline   (mg_pipeline_create_info *create_info);
void                    mg_opengl_destroy_pipeline  (mg_opengl_pipeline *pipeline);
void                    mg_opengl_bind_pipeline     (mg_opengl_pipeline *pipeline);