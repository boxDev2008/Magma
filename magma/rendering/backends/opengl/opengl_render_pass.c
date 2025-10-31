#include "opengl_render_pass.h"
#include "opengl_renderer.h"

mg_opengl_render_pass *mg_opengl_create_render_pass(mg_render_pass_create_info *create_info)
{
    mg_opengl_render_pass *render_pass = (mg_opengl_render_pass*)malloc(sizeof(mg_opengl_render_pass));
    glGenFramebuffers(1, &render_pass->framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, render_pass->framebuffer_id);

    mg_opengl_image *color_attachment = (mg_opengl_image*)create_info->color_attachment.image;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_attachment->texture_target, color_attachment->texture_id, 0);

    if (create_info->depth_stencil_attachment.image)
    {
        mg_opengl_image *depth_stencil_attachment = (mg_opengl_image*)create_info->depth_stencil_attachment.image;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, color_attachment->texture_target, depth_stencil_attachment->texture_id, 0);  
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return render_pass;
}

void mg_opengl_destroy_render_pass(mg_opengl_render_pass *render_pass)
{
    glDeleteFramebuffers(1, &render_pass->framebuffer_id);
    free(render_pass);
}

void mg_opengl_update_render_pass(mg_opengl_render_pass *render_pass, mg_render_pass_update_info *resize_info)
{
    glBindFramebuffer(GL_FRAMEBUFFER, render_pass->framebuffer_id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ((mg_opengl_image*)resize_info->color_image)->texture_id, 0);
    if (resize_info->depth_stencil_image)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, ((mg_opengl_image*)resize_info->depth_stencil_image)->texture_id, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void mg_opengl_begin_render_pass(mg_opengl_render_pass *render_pass, mg_render_pass_begin_info *begin_info)
{
    glBindFramebuffer(GL_FRAMEBUFFER, render_pass->framebuffer_id);
    glScissor(begin_info->render_area.x, begin_info->render_area.y, begin_info->render_area.z, begin_info->render_area.w);
    glClearColor(begin_info->clear_value.r, begin_info->clear_value.g, begin_info->clear_value.b, begin_info->clear_value.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void mg_opengl_begin_default_render_pass(mg_render_pass_begin_info *begin_info)
{
    glBindFramebuffer(GL_FRAMEBUFFER, gl_ctx.back_buffer.framebuffer);
    glScissor(begin_info->render_area.x, begin_info->render_area.y, begin_info->render_area.z, begin_info->render_area.w);
    glClearColor(begin_info->clear_value.r, begin_info->clear_value.g, begin_info->clear_value.b, begin_info->clear_value.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void mg_opengl_end_render_pass(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}