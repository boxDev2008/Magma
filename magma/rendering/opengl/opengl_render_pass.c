#include "opengl_render_pass.h"
#include "opengl_renderer.h"

#include <stdlib.h>
#include <assert.h>

mg_opengl_render_pass_t *mg_opengl_create_render_pass(mg_render_pass_create_info_t *create_info)
{
    return NULL;
}

void mg_opengl_destroy_render_pass(mg_opengl_render_pass_t *render_pass)
{
    
}

void mg_opengl_begin_render_pass(mg_opengl_render_pass_t *render_pass, mg_opengl_framebuffer_t *framebuffer, mg_render_pass_begin_info_t *begin_info)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
    glScissor(begin_info->render_area.x, begin_info->render_area.y, begin_info->render_area.z, begin_info->render_area.w);
    glClearColor(begin_info->clear_value.r, begin_info->clear_value.g, begin_info->clear_value.b, begin_info->clear_value.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void mg_opengl_begin_default_render_pass(mg_render_pass_begin_info_t *begin_info)
{
    glBindFramebuffer(GL_FRAMEBUFFER, opengl_context.back_buffer.framebuffer);
    glScissor(begin_info->render_area.x, begin_info->render_area.y, begin_info->render_area.z, begin_info->render_area.w);
    glClearColor(begin_info->clear_value.r, begin_info->clear_value.g, begin_info->clear_value.b, begin_info->clear_value.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void mg_opengl_end_render_pass(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}