#pragma once

#include "opengl_types.h"

mg_opengl_render_pass      *mg_opengl_create_render_pass        (const mg_render_pass_create_info *create_info);
void                        mg_opengl_destroy_render_pass       (mg_opengl_render_pass *render_pass);
void                        mg_opengl_update_render_pass        (mg_opengl_render_pass *render_pass, const mg_render_pass_update_info *update_info);

void                        mg_opengl_bind_render_pass          (mg_opengl_render_pass *render_pass, const mg_render_pass_bind_info *bind_info);