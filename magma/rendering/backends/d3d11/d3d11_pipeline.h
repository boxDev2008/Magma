#pragma once

#include "d3d11_renderer.h"

#if MG_PLATFORM_WINDOWS

mg_d3d11_pipeline  *mg_d3d11_create_pipeline    (const mg_pipeline_create_info *create_info);
void                mg_d3d11_destroy_pipeline   (mg_d3d11_pipeline *pipeline);
void                mg_d3d11_bind_pipeline      (mg_d3d11_pipeline *pipeline);

#endif