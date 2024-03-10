#pragma once

#include "opengl_types.h"

mg_opengl_render_pass_t    *mg_opengl_create_render_pass        (mg_render_pass_create_info_t *create_info);
void                        mg_opengl_destroy_render_pass       (mg_opengl_render_pass_t *render_pass);

void                        mg_opengl_begin_render_pass         (mg_opengl_render_pass_t *render_pass, mg_opengl_framebuffer_t *framebuffer, mg_render_pass_begin_info_t *begin_info);
void                        mg_opengl_begin_default_render_pass (mg_render_pass_begin_info_t *begin_info);

void                        mg_opengl_end_render_pass           (void);