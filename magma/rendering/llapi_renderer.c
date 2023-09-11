#include "llapi_renderer.h"

#include "vulkan/vulkan_renderer.h"
#include "vulkan/vulkan_swapchain.h"
#include "vulkan/vulkan_render_pass.h"
#include "vulkan/vulkan_pipeline.h"
#include "vulkan/vulkan_buffer.h"
#include "vulkan/vulkan_image.h"
#include "vulkan/vulkan_descriptor_set.h"

#include "opengl/opengl_renderer.h"

typedef struct mg_renderer_plugin mg_renderer_plugin_t;
struct mg_renderer_plugin
{
    mg_platform_t *platform;

    void (*initialize)      (mg_renderer_init_info_t *init_info);
    void (*shutdown)        (void);

    void (*begin_frame)     (void);
    void (*end_frame)       (void);

    void (*present_frame)   (void);
    void (*wait)            (void);

    void (*configure_swapchain) (mg_swapchain_config_info_t *config_info);

    void *(*get_swapchain_framebuffer)          (void);
    mg_vec2i_t (*get_swapchain_extent)          (void);
    mg_pixel_format_t (*get_swapchain_format)   (void);

    void (*viewport)        (uint32_t width, uint32_t height);

    void *(*create_render_pass) (void);
    void (*destroy_render_pass) (void *render_pass);
    void (*begin_render_pass)   (void *render_pass, mg_render_pass_begin_info_t *begin_info);
    void (*end_render_pass)     (void);

    void *(*create_descriptor_set_layout)   (mg_descriptor_set_layout_create_info_t *create_info);
    void (*destroy_descriptor_set_layout)   (void *descriptor_set_layout);

    void *(*create_descriptor_set)      (mg_descriptor_set_create_info_t *create_info);
    void (*update_descriptor_set)       (void *descriptor_set, mg_descriptor_write_t *descriptor_write);
    void (*destroy_descriptor_set)      (void *descriptor_set);

    void (*bind_descriptor_set)         (void *descriptor_set, void *pipeline, uint32_t set_index);

    void *(*create_pipeline)            (mg_pipeline_create_info_t *create_info);
    void (*destroy_pipeline)            (void *pipeline);
    void (*bind_pipeline)               (void *pipeline);

    void *(*create_buffer)              (mg_buffer_create_info_t *create_info);
    void (*update_buffer)               (void *buffer, mg_buffer_update_info_t *update_data);
    void (*destroy_buffer)              (void *buffer);

    void *(*create_texture_image)       (mg_texture_image_create_info_t *create_info);
    void *(*write_texture_image)        (void *texture_image, mg_texture_image_write_info_t *write_info);
    void (*destroy_texture_image)       (void *texture_image);

    void *(*create_texture_view)        (void *texture_image);
    void (*destroy_texture_view)        (void *texture_view);

    void *(*create_sampler)             (mg_sampler_create_info_t *create_info);
    void (*destroy_sampler)             (void *sampler);

    void *(*create_framebuffer)         (mg_framebuffer_create_info_t *create_info);
    void (*destroy_framebuffer)         (void *framebuffer);

    void (*bind_vertex_buffer)          (void *buffer);
    void (*bind_index_buffer)           (void *buffer, mg_index_type_t index_type);

    void (*draw)                        (uint32_t vertex_count, uint32_t first_vertex);
    void (*draw_indexed)                (uint32_t index_count, uint32_t first_index);

    void (*push_constants)              (void *pipeline, uint32_t size, void *data);
};

mg_renderer_plugin_t plugin;

void mg_llapi_renderer_initialize(mg_renderer_init_info_t *init_info)
{
    switch (init_info->type)
    {
        case MG_RENDERER_TYPE_OPENGL:
            plugin.initialize       =   mg_opengl_renderer_initialize;
            plugin.shutdown         =   mg_opengl_renderer_shutdown;
            plugin.present_frame    =   mg_opengl_renderer_present;
        break;
        case MG_RENDERER_TYPE_VULKAN:
            plugin.initialize       =   mg_vulkan_renderer_initialize;
            plugin.shutdown         =   mg_vulkan_renderer_shutdown;
            plugin.begin_frame      =   mg_vulkan_renderer_begin_frame;
            plugin.end_frame        =   mg_vulkan_renderer_end_frame;
            plugin.present_frame    =   mg_vulkan_renderer_present;
            plugin.wait             =   mg_vulkan_renderer_wait;
            plugin.viewport         =   mg_vulkan_renderer_viewport;

            plugin.configure_swapchain          =   mg_vulkan_configure_swapchain;
            plugin.get_swapchain_framebuffer    =   mg_vulkan_get_swapchain_framebuffer;

            plugin.create_render_pass   =   mg_vulkan_create_render_pass;
            plugin.destroy_render_pass  =   mg_vulkan_destroy_render_pass;
            plugin.begin_render_pass    =   mg_vulkan_begin_render_pass;
            plugin.end_render_pass      =   mg_vulkan_end_render_pass;

            plugin.create_descriptor_set_layout     =   mg_vulkan_create_descriptor_set_layout;
            plugin.destroy_descriptor_set_layout    =   mg_vulkan_destroy_descriptor_set_layout;

            plugin.create_descriptor_set    =   mg_vulkan_create_descriptor_set;
            plugin.update_descriptor_set    =   mg_vulkan_update_descriptor_set;
            plugin.destroy_descriptor_set   =   mg_vulkan_destroy_descriptor_set;

            plugin.bind_descriptor_set  =   mg_vulkan_bind_descriptor_set;

            plugin.create_pipeline   =   mg_vulkan_create_pipeline;
            plugin.destroy_pipeline  =   mg_vulkan_destroy_pipeline;
            plugin.bind_pipeline     =   mg_vulkan_bind_pipeline;

            plugin.create_buffer            =   mg_vulkan_create_buffer;
            plugin.update_buffer            =   mg_vulkan_update_buffer;
            plugin.destroy_buffer           =   mg_vulkan_destroy_buffer;

            plugin.create_texture_image     =   mg_vulkan_create_texture_image;
            plugin.write_texture_image      =   mg_vulkan_write_texture_image;
            plugin.destroy_texture_image    =   mg_vulkan_destroy_texture_image;

            plugin.create_texture_view      =   mg_vulkan_create_texture_view;
            plugin.destroy_texture_view     =   mg_vulkan_destroy_texture_view;

            plugin.create_sampler     =   mg_vulkan_create_sampler;
            plugin.destroy_sampler    =   mg_vulkan_destroy_sampler;

            plugin.create_framebuffer   =   mg_vulkan_create_framebuffer;
            plugin.destroy_framebuffer  =   mg_vulkan_destroy_framebuffer;

            plugin.bind_vertex_buffer   =   mg_vulkan_bind_vertex_buffer;
            plugin.bind_index_buffer    =   mg_vulkan_bind_index_buffer;

            plugin.draw         =   mg_vulkan_renderer_draw;
            plugin.draw_indexed =   mg_vulkan_renderer_draw_indexed;

            plugin.push_constants   =   mg_vulkan_renderer_push_constants;
        break;
    }

    plugin.initialize(init_info);
}

void mg_llapi_renderer_shutdown(void)
{
    plugin.shutdown();
}

void mg_llapi_renderer_begin_frame(void)
{
    plugin.begin_frame();
}

void mg_llapi_renderer_end_frame(void)
{
    plugin.end_frame();
}

void mg_llapi_renderer_present_frame(void)
{
    plugin.present_frame();
}

void mg_llapi_renderer_wait(void)
{
    plugin.wait();
}

void mg_llapi_renderer_viewport(uint32_t width, uint32_t height)
{
    plugin.viewport(width, height);
}

void mg_llapi_renderer_configure_swapchain(mg_swapchain_config_info_t *config_info)
{
    plugin.configure_swapchain(config_info);
}

mg_framebuffer_t mg_llapi_renderer_get_swapchain_framebuffer(void)
{
    return (mg_framebuffer_t) { plugin.get_swapchain_framebuffer() };
}

mg_render_pass_t mg_llapi_renderer_create_render_pass(void)
{
    mg_render_pass_t render_pass;
    render_pass.internal_data = plugin.create_render_pass();
    return render_pass;
}

void mg_llapi_renderer_destroy_render_pass(mg_render_pass_t render_pass)
{
    plugin.destroy_render_pass(render_pass.internal_data);
}

void mg_llapi_renderer_begin_render_pass(mg_render_pass_t render_pass, mg_render_pass_begin_info_t *begin_info)
{
    plugin.begin_render_pass(render_pass.internal_data, begin_info);
}

void mg_llapi_renderer_end_render_pass(void)
{
    plugin.end_render_pass();
}

void mg_llapi_renderer_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    plugin.draw(vertex_count, first_vertex);
}

void mg_llapi_renderer_draw_indexed(uint32_t index_count, uint32_t first_index)
{
    plugin.draw_indexed(index_count, first_index);
}

mg_descriptor_set_layout_t mg_llapi_renderer_create_descriptor_set_layout(mg_descriptor_set_layout_create_info_t *create_info)
{
    mg_descriptor_set_layout_t descriptor_set_layout;
    descriptor_set_layout.internal_data = plugin.create_descriptor_set_layout(create_info);
    return descriptor_set_layout;
}

void mg_llapi_renderer_destroy_descriptor_set_layout(mg_descriptor_set_layout_t descriptor_set_layout)
{
    plugin.destroy_descriptor_set_layout(descriptor_set_layout.internal_data);
}

mg_descriptor_set_t mg_llapi_renderer_create_descriptor_set(mg_descriptor_set_create_info_t *create_info)
{
    mg_descriptor_set_t descriptor_set;
    descriptor_set.internal_data = plugin.create_descriptor_set(create_info);
    return descriptor_set;
}

void mg_llapi_renderer_update_descriptor_set(mg_descriptor_set_t descriptor_set, mg_descriptor_write_t *descriptor_write)
{
    plugin.update_descriptor_set(descriptor_set.internal_data, descriptor_write);
}

void mg_llapi_renderer_destroy_descriptor_set(mg_descriptor_set_t descriptor_set)
{
    plugin.destroy_descriptor_set(descriptor_set.internal_data);
}

void mg_llapi_renderer_bind_descriptor_set(mg_descriptor_set_t descriptor_set, mg_pipeline_t pipeline, uint32_t set_index)
{
    plugin.bind_descriptor_set(descriptor_set.internal_data, pipeline.internal_data, set_index);
}

mg_pipeline_t mg_llapi_renderer_create_pipeline(mg_pipeline_create_info_t *create_info)
{
    mg_pipeline_t pipeline;
    pipeline.internal_data = plugin.create_pipeline(create_info);
    return pipeline;
}

void mg_llapi_renderer_destroy_pipeline(mg_pipeline_t pipeline)
{
    plugin.destroy_pipeline(pipeline.internal_data);
}

void mg_llapi_renderer_bind_pipeline(mg_pipeline_t pipeline)
{
    plugin.bind_pipeline(pipeline.internal_data);
}

mg_buffer_t mg_llapi_renderer_create_buffer(mg_buffer_create_info_t *create_info)
{
    mg_buffer_t buffer;
    buffer.internal_data = plugin.create_buffer(create_info);
    return buffer;
}

void mg_llapi_renderer_destroy_buffer(mg_buffer_t buffer)
{
    plugin.destroy_buffer(buffer.internal_data);
}

void mg_llapi_renderer_update_buffer(mg_buffer_t buffer, mg_buffer_update_info_t *update_info)
{
    plugin.update_buffer(buffer.internal_data, update_info);
}

mg_texture_image_t mg_llapi_renderer_create_texture_image(mg_texture_image_create_info_t *create_info)
{
    mg_texture_image_t texture_image;
    texture_image.internal_data = plugin.create_texture_image(create_info);
    return texture_image;
}

void mg_llapi_renderer_write_texture_image(mg_texture_image_t texture_image, mg_texture_image_write_info_t *write_info)
{
    plugin.write_texture_image(texture_image.internal_data, write_info);
}

void mg_llapi_renderer_destroy_texture_image(mg_texture_image_t texture_image)
{
    plugin.destroy_texture_image(texture_image.internal_data);
}

mg_texture_view_t mg_llapi_renderer_create_texture_view(mg_texture_image_t texture_image)
{
    mg_texture_view_t view;
    view.internal_data = plugin.create_texture_view(texture_image.internal_data);
    return view;
}

void mg_llapi_renderer_destroy_texture_view(mg_texture_view_t texture_view)
{
    plugin.destroy_texture_view(texture_view.internal_data);
}

mg_sampler_t mg_llapi_renderer_create_sampler(mg_sampler_create_info_t *create_info)
{
    mg_sampler_t sampler;
    sampler.internal_data = plugin.create_sampler(create_info);
    return sampler;
}

mg_framebuffer_t mg_llapi_renderer_create_framebuffer(mg_framebuffer_create_info_t *create_info)
{
    mg_framebuffer_t framebuffer;
    framebuffer.internal_data = plugin.create_framebuffer(create_info);
    return framebuffer;
}

void mg_llapi_renderer_destroy_framebuffer(mg_framebuffer_t framebuffer)
{
    plugin.destroy_framebuffer(framebuffer.internal_data);
}

void mg_llapi_renderer_destroy_sampler(mg_sampler_t sampler)
{
    plugin.destroy_sampler(sampler.internal_data);
}

void mg_llapi_renderer_bind_vertex_buffer(mg_buffer_t buffer)
{
    plugin.bind_vertex_buffer(buffer.internal_data);
}

void mg_llapi_renderer_bind_index_buffer(mg_buffer_t buffer, mg_index_type_t index_type)
{
    plugin.bind_index_buffer(buffer.internal_data, index_type);
}

void mg_llapi_renderer_push_constants(mg_pipeline_t pipeline, uint32_t size, void *data)
{
    plugin.push_constants(pipeline.internal_data, size, data);
}