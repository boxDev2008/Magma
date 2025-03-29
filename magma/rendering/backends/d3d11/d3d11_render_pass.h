#pragma once

#include "d3d11_types.h"

#if MG_PLATFORM_WINDOWS

mg_d3d11_render_pass *mg_d3d11_create_render_pass(mg_render_pass_create_info *create_info);
void mg_d3d11_destroy_render_pass(mg_d3d11_render_pass *render_pass);

void mg_d3d11_begin_render_pass(mg_d3d11_render_pass *render_pass, mg_d3d11_framebuffer *framebuffer, mg_render_pass_begin_info *begin_info);
void mg_d3d11_begin_default_render_pass(mg_render_pass_begin_info *begin_info);
void mg_d3d11_end_render_pass(void);

#endif