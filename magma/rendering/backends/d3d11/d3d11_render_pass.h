#pragma once

#include "d3d11_types.h"

#if MG_PLATFORM_WINDOWS

mg_d3d11_render_pass *mg_d3d11_create_render_pass(mg_render_pass_create_info *create_info);
void mg_d3d11_destroy_render_pass(mg_d3d11_render_pass *render_pass);
void mg_d3d11_update_render_pass(mg_d3d11_render_pass *render_pass, mg_render_pass_update_info *resize_info);

void mg_d3d11_bind_render_pass(mg_d3d11_render_pass *render_pass, mg_render_pass_bind_info *bind_info);

#endif