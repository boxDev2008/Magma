#include "rhi_renderer.h"

#include "vulkan/vulkan_renderer.h"
#include "vulkan/vulkan_swapchain.h"
#include "vulkan/vulkan_render_pass.h"
#include "vulkan/vulkan_pipeline.h"
#include "vulkan/vulkan_buffer.h"
#include "vulkan/vulkan_image.h"

#include "opengl/opengl_renderer.h"
#include "opengl/opengl_buffer.h"
#include "opengl/opengl_pipeline.h"
#include "opengl/opengl_image.h"
#include "opengl/opengl_render_pass.h"

typedef struct mg_rhi_renderer_plugin
{
    mg_platform_t *platform;

    void (*initialize)      (mg_renderer_init_info_t *init_info);
    void (*shutdown)        (void);

    void (*begin_frame)     (void);
    void (*end_frame)       (void);

    void (*present_frame)   (void);
    void (*wait)            (void);

    void (*configure_swapchain) (mg_swapchain_config_info_t *config_info);

    void (*viewport)        (int32_t x, int32_t y, uint32_t width, uint32_t height);
    void (*scissor)         (int32_t x, int32_t y, uint32_t width, uint32_t height);

    void *(*create_render_pass)         (mg_render_pass_create_info_t *create_info);
    void (*destroy_render_pass)         (void *render_pass);
    void (*begin_render_pass)           (void *render_pass, void *framebuffer, mg_render_pass_begin_info_t *begin_info);
    void (*begin_default_render_pass)   (mg_render_pass_begin_info_t *begin_info);
    void (*end_render_pass)             (void);

    void *(*create_pipeline)            (mg_pipeline_create_info_t *create_info);
    void (*destroy_pipeline)            (void *pipeline);
    void (*bind_pipeline)               (void *pipeline);

    void *(*create_image)               (mg_image_create_info_t *create_info);
    void (*destroy_image)               (void *image);
    void (*write_image)                 (void *image, mg_image_write_info_t *write_info);
    void (*update_image)                (void *image, void *sampler, uint32_t binding);
    void (*bind_image)                  (void *image, void *pipeline);

    void *(*create_sampler)             (mg_sampler_create_info_t *create_info);
    void (*destroy_sampler)             (void *sampler);

    void *(*create_framebuffer)         (mg_framebuffer_create_info_t *create_info);
    void (*destroy_framebuffer)         (void *framebuffer);

    void *(*create_vertex_buffer)           (size_t size, void *data);
    void (*destroy_vertex_buffer)           (void *buffer);

    void *(*create_index_buffer)            (size_t size, void *data);
    void (*destroy_index_buffer)            (void *buffer);

    void *(*create_dynamic_vertex_buffer)   (size_t size);
    void (*destroy_dynamic_vertex_buffer)   (void *buffer);
    void (*update_dynamic_vertex_buffer)    (void *buffer, size_t size, void *data);

    void *(*create_dynamic_index_buffer)    (size_t size);
    void (*destroy_dynamic_index_buffer)    (void *buffer);
    void (*update_dynamic_index_buffer)     (void *buffer, size_t size, void *data);

    void *(*create_uniform_buffer)          (size_t size);
    void (*destroy_uniform_buffer)          (void *buffer);
    void (*update_uniform_buffer)           (void *buffer, size_t size, uint32_t binding, void *data);

    void (*bind_vertex_buffer)              (void *buffer);
    void (*bind_dynamic_vertex_buffer)      (void *buffer);
    void (*bind_index_buffer)               (void *buffer, mg_index_type_t index_type);
    void (*bind_dynamic_index_buffer)       (void *buffer, mg_index_type_t index_type);
    void (*bind_unifom_buffer)              (void *buffer, void *pipeline);

    void (*draw)                        (uint32_t vertex_count, uint32_t first_vertex);
    void (*draw_indexed)                (uint32_t index_count, uint32_t first_index);

    void (*push_constants)              (void *pipeline, uint32_t size, void *data);
}
mg_rhi_renderer_plugin_t;

mg_rhi_renderer_plugin_t plugin;

void mg_rhi_renderer_initialize(mg_renderer_init_info_t *init_info)
{
    switch (init_info->type)
    {
        case MG_RENDERER_TYPE_VULKAN:
            plugin.initialize       =   mg_vulkan_renderer_initialize;
            plugin.shutdown         =   mg_vulkan_renderer_shutdown;
            plugin.begin_frame      =   mg_vulkan_renderer_begin_frame;
            plugin.end_frame        =   mg_vulkan_renderer_end_frame;
            plugin.present_frame    =   mg_vulkan_renderer_present_frame;
            plugin.wait             =   mg_vulkan_renderer_wait;
            plugin.viewport         =   mg_vulkan_renderer_viewport;
            plugin.scissor          =   mg_vulkan_renderer_scissor;

            plugin.configure_swapchain          =   mg_vulkan_configure_swapchain;

            plugin.create_render_pass           =   mg_vulkan_create_render_pass;
            plugin.destroy_render_pass          =   mg_vulkan_destroy_render_pass;
            plugin.begin_render_pass            =   mg_vulkan_begin_render_pass;
            plugin.begin_default_render_pass    =   mg_vulkan_begin_default_render_pass;
            plugin.end_render_pass              =   mg_vulkan_end_render_pass;

            plugin.create_pipeline   =   mg_vulkan_create_pipeline;
            plugin.destroy_pipeline  =   mg_vulkan_destroy_pipeline;
            plugin.bind_pipeline     =   mg_vulkan_bind_pipeline;

            plugin.create_vertex_buffer             =   mg_vulkan_create_vertex_buffer;
            plugin.destroy_vertex_buffer            =   mg_vulkan_destroy_vertex_buffer;

            plugin.create_index_buffer              =   mg_vulkan_create_index_buffer;
            plugin.destroy_index_buffer             =   mg_vulkan_destroy_index_buffer;

            plugin.create_dynamic_vertex_buffer     =   mg_vulkan_create_dynamic_vertex_buffer;
            plugin.destroy_dynamic_vertex_buffer    =   mg_vulkan_destroy_dynamic_vertex_buffer;
            plugin.update_dynamic_vertex_buffer     =   mg_vulkan_update_dynamic_vertex_buffer;
    
            plugin.create_dynamic_index_buffer      =   mg_vulkan_create_dynamic_index_buffer;
            plugin.destroy_dynamic_index_buffer     =   mg_vulkan_destroy_dynamic_index_buffer;
            plugin.update_dynamic_index_buffer      =   mg_vulkan_update_dynamic_index_buffer;

            plugin.create_uniform_buffer            =   mg_vulkan_create_uniform_buffer;
            plugin.destroy_uniform_buffer           =   mg_vulkan_destroy_uniform_buffer;
            plugin.update_uniform_buffer            =   mg_vulkan_update_uniform_buffer;

            plugin.bind_vertex_buffer           =   mg_vulkan_bind_vertex_buffer;
            plugin.bind_dynamic_vertex_buffer   =   mg_vulkan_bind_dynamic_vertex_buffer;
            plugin.bind_index_buffer            =   mg_vulkan_bind_index_buffer;
            plugin.bind_dynamic_index_buffer    =   mg_vulkan_bind_dynamic_index_buffer;
            plugin.bind_unifom_buffer           =   mg_vulkan_bind_unifom_buffer;

            plugin.create_image     =   mg_vulkan_create_image;
            plugin.destroy_image    =   mg_vulkan_destroy_image;
            plugin.write_image      =   mg_vulkan_write_image;
            plugin.update_image     =   mg_vulkan_update_image;
            plugin.bind_image       =   mg_vulkan_bind_image;

            plugin.create_sampler     =   mg_vulkan_create_sampler;
            plugin.destroy_sampler    =   mg_vulkan_destroy_sampler;

            plugin.create_framebuffer   =   mg_vulkan_create_framebuffer;
            plugin.destroy_framebuffer  =   mg_vulkan_destroy_framebuffer;

            plugin.draw         =   mg_vulkan_renderer_draw;
            plugin.draw_indexed =   mg_vulkan_renderer_draw_indexed;

            plugin.push_constants   =   mg_vulkan_renderer_push_constants;
        break;
        case MG_RENDERER_TYPE_OPENGL:
            plugin.initialize       =   mg_opengl_renderer_initialize;
            plugin.shutdown         =   mg_opengl_renderer_shutdown;
            plugin.begin_frame      =   mg_opengl_renderer_begin_frame;
            plugin.end_frame        =   mg_opengl_renderer_end_frame;
            plugin.present_frame    =   mg_opengl_renderer_present_frame;
            plugin.wait             =   mg_opengl_renderer_wait;
            plugin.viewport         =   mg_opengl_renderer_viewport;
            plugin.scissor          =   mg_opengl_renderer_scissor;

            plugin.configure_swapchain          =   mg_opengl_configure_swapchain;

            plugin.create_render_pass           =   mg_opengl_create_render_pass;
            plugin.destroy_render_pass          =   mg_opengl_destroy_render_pass;
            plugin.begin_render_pass            =   mg_opengl_begin_render_pass;
            plugin.begin_default_render_pass    =   mg_opengl_begin_default_render_pass;
            plugin.end_render_pass              =   mg_opengl_end_render_pass;

            plugin.create_pipeline  =   mg_opengl_create_pipeline;
            plugin.destroy_pipeline =   mg_opengl_destroy_pipeline;
            plugin.bind_pipeline    =   mg_opengl_bind_pipeline;

            plugin.create_vertex_buffer             =   mg_opengl_create_vertex_buffer;
            plugin.destroy_vertex_buffer            =   mg_opengl_destroy_vertex_buffer;

            plugin.create_index_buffer              =   mg_opengl_create_index_buffer;
            plugin.destroy_index_buffer             =   mg_opengl_destroy_index_buffer;

            plugin.create_dynamic_vertex_buffer     =   mg_opengl_create_dynamic_vertex_buffer;
            plugin.destroy_dynamic_vertex_buffer    =   mg_opengl_destroy_dynamic_vertex_buffer;
            plugin.update_dynamic_vertex_buffer     =   mg_opengl_update_dynamic_vertex_buffer;

            plugin.create_dynamic_index_buffer      =   mg_opengl_create_dynamic_index_buffer;
            plugin.destroy_dynamic_index_buffer     =   mg_opengl_destroy_dynamic_index_buffer;
            plugin.update_dynamic_index_buffer      =   mg_opengl_update_dynamic_index_buffer;

            plugin.create_uniform_buffer            =   mg_opengl_create_uniform_buffer;
            plugin.destroy_uniform_buffer           =   mg_opengl_destroy_uniform_buffer;
            plugin.update_uniform_buffer            =   mg_opengl_update_uniform_buffer;

            plugin.bind_vertex_buffer               =   mg_opengl_bind_vertex_buffer;
            plugin.bind_dynamic_vertex_buffer       =   mg_opengl_bind_dynamic_vertex_buffer;
            plugin.bind_index_buffer                =   mg_opengl_bind_index_buffer;
            plugin.bind_dynamic_index_buffer        =   mg_opengl_bind_dynamic_index_buffer;
            plugin.bind_unifom_buffer               =   mg_opengl_bind_unifom_buffer;

            plugin.create_image     =   mg_opengl_create_image;
            plugin.destroy_image    =   mg_opengl_destroy_image;
            plugin.write_image      =   mg_opengl_write_image;
            plugin.update_image     =   mg_opengl_update_image;
            plugin.bind_image       =   mg_opengl_bind_image;

            plugin.create_sampler     =   mg_opengl_create_sampler;
            plugin.destroy_sampler    =   mg_opengl_destroy_sampler;

            plugin.create_framebuffer   =   mg_opengl_create_framebuffer;
            plugin.destroy_framebuffer  =   mg_opengl_destroy_framebuffer;

            plugin.draw         =   mg_opengl_renderer_draw;
            plugin.draw_indexed =   mg_opengl_renderer_draw_indexed;

            plugin.push_constants = mg_opengl_renderer_push_constants;
        break;
    }

    plugin.initialize(init_info);
}

void mg_rhi_renderer_shutdown(void)
{
    plugin.shutdown();
}

void mg_rhi_renderer_begin_frame(void)
{
    plugin.begin_frame();
}

void mg_rhi_renderer_end_frame(void)
{
    plugin.end_frame();
}

void mg_rhi_renderer_present_frame(void)
{
    plugin.present_frame();
}

void mg_rhi_renderer_wait(void)
{
    plugin.wait();
}

void mg_rhi_renderer_viewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    plugin.viewport(x, y, width, height);
}

void mg_rhi_renderer_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    plugin.scissor(x, y, width, height);
}

void mg_rhi_renderer_configure_swapchain(mg_swapchain_config_info_t *config_info)
{
    plugin.configure_swapchain(config_info);
}

mg_render_pass_t mg_rhi_renderer_create_render_pass(mg_render_pass_create_info_t *create_info)
{
    mg_render_pass_t render_pass;
    render_pass.internal_data = plugin.create_render_pass(create_info);
    return render_pass;
}

void mg_rhi_renderer_destroy_render_pass(mg_render_pass_t render_pass)
{
    plugin.destroy_render_pass(render_pass.internal_data);
}

void mg_rhi_renderer_begin_render_pass(mg_render_pass_t render_pass, mg_framebuffer_t framebuffer, mg_render_pass_begin_info_t *begin_info)
{
    plugin.begin_render_pass(render_pass.internal_data, framebuffer.internal_data, begin_info);
}

void mg_rhi_renderer_begin_default_render_pass(mg_render_pass_begin_info_t *begin_info)
{
    plugin.begin_default_render_pass(begin_info);
}

void mg_rhi_renderer_end_render_pass(void)
{
    plugin.end_render_pass();
}

void mg_rhi_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    plugin.draw(vertex_count, first_vertex);
}

void mg_rhi_renderer_draw_indexed(uint32_t index_count, uint32_t first_index)
{
    plugin.draw_indexed(index_count, first_index);
}

mg_pipeline_t mg_rhi_renderer_create_pipeline(mg_pipeline_create_info_t *create_info)
{
    mg_pipeline_t pipeline;
    pipeline.internal_data = plugin.create_pipeline(create_info);
    return pipeline;
}

void mg_rhi_renderer_destroy_pipeline(mg_pipeline_t pipeline)
{
    plugin.destroy_pipeline(pipeline.internal_data);
}

void mg_rhi_renderer_bind_pipeline(mg_pipeline_t pipeline)
{
    plugin.bind_pipeline(pipeline.internal_data);
}

mg_vertex_buffer_t mg_rhi_renderer_create_vertex_buffer(size_t size, void *data)
{
    mg_vertex_buffer_t buffer;
    buffer.internal_data = plugin.create_vertex_buffer(size, data);
    return buffer;
}

void mg_rhi_renderer_destroy_vertex_buffer(mg_vertex_buffer_t buffer)
{
    plugin.destroy_vertex_buffer(buffer.internal_data);
}

mg_index_buffer_t mg_rhi_renderer_create_index_buffer(size_t size, void *data)
{
    mg_index_buffer_t buffer;
    buffer.internal_data = plugin.create_index_buffer(size, data);
    return buffer;
}

void mg_rhi_renderer_destroy_index_buffer(mg_index_buffer_t buffer)
{
    plugin.destroy_index_buffer(buffer.internal_data);
}

mg_dynamic_vertex_buffer_t mg_rhi_renderer_create_dynamic_vertex_buffer(size_t size)
{
    mg_dynamic_vertex_buffer_t buffer;
    buffer.internal_data = plugin.create_dynamic_vertex_buffer(size);
    return buffer;
}

void mg_rhi_renderer_destroy_dynamic_vertex_buffer(mg_dynamic_vertex_buffer_t buffer)
{
    plugin.destroy_dynamic_vertex_buffer(buffer.internal_data);
}

void mg_rhi_renderer_update_dynamic_vertex_buffer(mg_dynamic_vertex_buffer_t buffer, size_t size, void *data)
{
    plugin.update_dynamic_vertex_buffer(buffer.internal_data, size, data);
}

mg_dynamic_index_buffer_t mg_rhi_renderer_create_dynamic_index_buffer(size_t size)
{
    mg_dynamic_index_buffer_t buffer;
    buffer.internal_data = plugin.create_dynamic_index_buffer(size);
    return buffer;
}

void mg_rhi_renderer_destroy_dynamic_index_buffer(mg_dynamic_index_buffer_t buffer)
{
    plugin.destroy_dynamic_index_buffer(buffer.internal_data);
}

void mg_rhi_renderer_update_dynamic_index_buffer(mg_dynamic_index_buffer_t buffer, size_t size, void *data)
{
    plugin.update_dynamic_index_buffer(buffer.internal_data, size, data);
}

mg_uniform_buffer_t mg_rhi_renderer_create_uniform_buffer(size_t size)
{
    mg_uniform_buffer_t buffer;
    buffer.internal_data = plugin.create_uniform_buffer(size);
    return buffer;
}

void mg_rhi_renderer_destroy_uniform_buffer(mg_uniform_buffer_t buffer)
{
    plugin.destroy_uniform_buffer(buffer.internal_data);
}

void mg_rhi_renderer_update_uniform_buffer(mg_uniform_buffer_t buffer, size_t size, uint32_t binding, void *data)
{
    plugin.update_uniform_buffer(buffer.internal_data, size, binding, data);
}

void mg_rhi_renderer_bind_vertex_buffer(mg_vertex_buffer_t buffer)
{
    plugin.bind_vertex_buffer(buffer.internal_data);
}

void mg_rhi_renderer_bind_dynamic_vertex_buffer(mg_dynamic_vertex_buffer_t buffer)
{
    plugin.bind_dynamic_vertex_buffer(buffer.internal_data);
}

void mg_rhi_renderer_bind_index_buffer(mg_index_buffer_t buffer, mg_index_type_t index_type)
{
    plugin.bind_index_buffer(buffer.internal_data, index_type);
}

void mg_rhi_renderer_bind_dynamic_index_buffer(mg_index_buffer_t buffer, mg_index_type_t index_type)
{
    plugin.bind_dynamic_index_buffer(buffer.internal_data, index_type);
}

void mg_rhi_renderer_bind_uniform_buffer(mg_uniform_buffer_t buffer, mg_pipeline_t pipeline)
{
    plugin.bind_unifom_buffer(buffer.internal_data, pipeline.internal_data);
}

mg_image_t mg_rhi_renderer_create_image(mg_image_create_info_t *create_info)
{
    mg_image_t image;
    image.internal_data = plugin.create_image(create_info);
    return image;
}

void mg_rhi_renderer_destroy_image(mg_image_t image)
{
    plugin.destroy_image(image.internal_data);
}

void mg_rhi_renderer_write_image(mg_image_t image, mg_image_write_info_t *write_info)
{
    plugin.write_image(image.internal_data, write_info);
}

void mg_rhi_renderer_update_image(mg_image_t image, mg_sampler_t sampler, uint32_t binding)
{
    plugin.update_image(image.internal_data, sampler.internal_data, binding);
}

void mg_rhi_renderer_bind_image(mg_image_t image, mg_pipeline_t pipeline)
{
    plugin.bind_image(image.internal_data, pipeline.internal_data);
}

mg_sampler_t mg_rhi_renderer_create_sampler(mg_sampler_create_info_t *create_info)
{
    mg_sampler_t sampler;
    sampler.internal_data = plugin.create_sampler(create_info);
    return sampler;
}

mg_framebuffer_t mg_rhi_renderer_create_framebuffer(mg_framebuffer_create_info_t *create_info)
{
    mg_framebuffer_t framebuffer;
    framebuffer.internal_data = plugin.create_framebuffer(create_info);
    return framebuffer;
}

void mg_rhi_renderer_destroy_framebuffer(mg_framebuffer_t framebuffer)
{
    plugin.destroy_framebuffer(framebuffer.internal_data);
}

void mg_rhi_renderer_destroy_sampler(mg_sampler_t sampler)
{
    plugin.destroy_sampler(sampler.internal_data);
}

void mg_rhi_renderer_push_constants(mg_pipeline_t pipeline, uint32_t size, void *data)
{
    plugin.push_constants(pipeline.internal_data, size, data);
}