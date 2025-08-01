#include "renderer.h"

#include "backends/opengl/opengl_renderer.h"
#include "backends/opengl/opengl_buffer.h"
#include "backends/opengl/opengl_pipeline.h"
#include "backends/opengl/opengl_image.h"
#include "backends/opengl/opengl_render_pass.h"

#if !defined(MG_PLATFORM_EMSCRIPTEN)
    #include "backends/vulkan/vulkan_renderer.h"
    #include "backends/vulkan/vulkan_swapchain.h"
    #include "backends/vulkan/vulkan_render_pass.h"
    #include "backends/vulkan/vulkan_pipeline.h"
    #include "backends/vulkan/vulkan_buffer.h"
    #include "backends/vulkan/vulkan_image.h"
#endif

#if MG_PLATFORM_WINDOWS
    #include "backends/d3d11/d3d11_renderer.h"
    #include "backends/d3d11/d3d11_buffer.h"
    #include "backends/d3d11/d3d11_pipeline.h"
    #include "backends/d3d11/d3d11_render_pass.h"
    #include "backends/d3d11/d3d11_image.h"
#endif

typedef struct mgfx_pipe
{
    mg_platform *platform;
    mg_renderer_type type;

    void (*initialize)      (mgfx_init_info *init_info);
    void (*shutdown)        (void);

    void (*begin)     (void);
    void (*end)       (void);

    void (*present_frame)   (void);
    void (*wait)            (void);

    void (*configure_swapchain) (mg_swapchain_config_info *config_info);

    void (*viewport)        (int32_t x, int32_t y, uint32_t width, uint32_t height);
    void (*scissor)         (int32_t x, int32_t y, uint32_t width, uint32_t height);

    void *(*create_render_pass)         (mg_render_pass_create_info *create_info);
    void (*destroy_render_pass)         (void *render_pass);
    void (*begin_render_pass)           (void *render_pass, void *framebuffer, mg_render_pass_begin_info *begin_info);
    void (*begin_default_render_pass)   (mg_render_pass_begin_info *begin_info);
    void (*end_render_pass)             (void);

    void *(*create_pipeline)            (mg_pipeline_create_info *create_info);
    void (*destroy_pipeline)            (void *pipeline);
    void (*bind_pipeline)               (void *pipeline);

    void *(*create_image)               (mg_image_create_info *create_info);
    void (*destroy_image)               (void *image);
    void (*update_image)                (void *image, mg_image_write_info *write_info);

    mg_image_array (*create_image_array)    (void);
    void (*destroy_image_array)             (mg_image_array array);
    void (*update_image_array)              (mg_image_array array, mg_image *images, mg_sampler *samplers, uint32_t count);
    void (*bind_image_array)                (mg_image_array array);

    void *(*create_sampler)             (mg_sampler_create_info *create_info);
    void (*destroy_sampler)             (void *sampler);

    void *(*create_framebuffer)         (mg_framebuffer_create_info *create_info);
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

    void (*bind_vertex_buffer)              (void *buffer);
    void (*bind_dynamic_vertex_buffer)      (void *buffer);
    void (*bind_index_buffer)               (void *buffer, mg_index_type index_type);
    void (*bind_dynamic_index_buffer)       (void *buffer, mg_index_type index_type);

    void (*bind_uniforms)                   (uint32_t binding, size_t size, void *data);

    void (*draw)                        (uint32_t vertex_count, uint32_t first_vertex);
    void (*draw_indexed)                (uint32_t index_count, uint32_t first_index, uint32_t vertex_offset);
}
mgfx_pipe;

static mgfx_pipe pipe;

void mgfx_initialize(mgfx_init_info *init_info)
{
    switch (init_info->type)
    {
#if !defined(MG_PLATFORM_EMSCRIPTEN)
    case MG_RENDERER_TYPE_VULKAN:
        pipe.initialize       =   mg_vulkan_renderer_initialize;
        pipe.shutdown         =   mg_vulkan_renderer_shutdown;
        pipe.begin      =   mg_vulkan_renderer_begin;
        pipe.end        =   mg_vulkan_renderer_end;
        pipe.wait             =   mg_vulkan_renderer_wait;
        pipe.viewport         =   mg_vulkan_renderer_viewport;
        pipe.scissor          =   mg_vulkan_renderer_scissor;

        pipe.configure_swapchain          =   mg_vulkan_configure_swapchain;

        pipe.create_render_pass           =   mg_vulkan_create_render_pass;
        pipe.destroy_render_pass          =   mg_vulkan_destroy_render_pass;
        pipe.begin_render_pass            =   mg_vulkan_begin_render_pass;
        pipe.begin_default_render_pass    =   mg_vulkan_begin_default_render_pass;
        pipe.end_render_pass              =   mg_vulkan_end_render_pass;

        pipe.create_pipeline   =   mg_vulkan_create_pipeline;
        pipe.destroy_pipeline  =   mg_vulkan_destroy_pipeline;
        pipe.bind_pipeline     =   mg_vulkan_bind_pipeline;

        pipe.create_vertex_buffer             =   mg_vulkan_create_vertex_buffer;
        pipe.destroy_vertex_buffer            =   mg_vulkan_destroy_vertex_buffer;

        pipe.create_index_buffer              =   mg_vulkan_create_index_buffer;
        pipe.destroy_index_buffer             =   mg_vulkan_destroy_index_buffer;

        pipe.create_dynamic_vertex_buffer     =   mg_vulkan_create_dynamic_vertex_buffer;
        pipe.destroy_dynamic_vertex_buffer    =   mg_vulkan_destroy_dynamic_vertex_buffer;
        pipe.update_dynamic_vertex_buffer     =   mg_vulkan_update_dynamic_vertex_buffer;

        pipe.create_dynamic_index_buffer      =   mg_vulkan_create_dynamic_index_buffer;
        pipe.destroy_dynamic_index_buffer     =   mg_vulkan_destroy_dynamic_index_buffer;
        pipe.update_dynamic_index_buffer      =   mg_vulkan_update_dynamic_index_buffer;

        pipe.bind_vertex_buffer           =   mg_vulkan_bind_vertex_buffer;
        pipe.bind_dynamic_vertex_buffer   =   mg_vulkan_bind_dynamic_vertex_buffer;
        pipe.bind_index_buffer            =   mg_vulkan_bind_index_buffer;
        pipe.bind_dynamic_index_buffer    =   mg_vulkan_bind_dynamic_index_buffer;
        
        pipe.bind_uniforms                =   mg_vulkan_renderer_bind_uniforms;

        pipe.create_image     =   mg_vulkan_create_image;
        pipe.destroy_image    =   mg_vulkan_destroy_image;
        pipe.update_image     =   mg_vulkan_update_image;
        //pipe.bind_image       =   mg_vulkan_bind_image;

        pipe.create_sampler     =   mg_vulkan_create_sampler;
        pipe.destroy_sampler    =   mg_vulkan_destroy_sampler;

        pipe.create_image_array = mg_vulkan_create_image_array;
        pipe.destroy_image_array = mg_vulkan_destroy_image_array;
        pipe.update_image_array = mg_vulkan_update_image_array;
        pipe.bind_image_array = mg_vulkan_bind_image_array;

        pipe.create_framebuffer   =   mg_vulkan_create_framebuffer;
        pipe.destroy_framebuffer  =   mg_vulkan_destroy_framebuffer;

        pipe.draw         =   mg_vulkan_renderer_draw;
        pipe.draw_indexed =   mg_vulkan_renderer_draw_indexed;
        break;
#endif
    case MG_RENDERER_TYPE_OPENGL:
    case MG_RENDERER_TYPE_OPENGLES:
        pipe.initialize       =   mg_opengl_renderer_initialize;
        pipe.shutdown         =   mg_opengl_renderer_shutdown;
        pipe.begin      =   mg_opengl_renderer_begin;
        pipe.end        =   mg_opengl_renderer_end;
        pipe.wait             =   mg_opengl_renderer_wait;
        pipe.viewport         =   mg_opengl_renderer_viewport;
        pipe.scissor          =   mg_opengl_renderer_scissor;

        pipe.configure_swapchain          =   mg_opengl_configure_swapchain;

        pipe.create_render_pass           =   mg_opengl_create_render_pass;
        pipe.destroy_render_pass          =   mg_opengl_destroy_render_pass;
        pipe.begin_render_pass            =   mg_opengl_begin_render_pass;
        pipe.begin_default_render_pass    =   mg_opengl_begin_default_render_pass;
        pipe.end_render_pass              =   mg_opengl_end_render_pass;

        pipe.create_pipeline  =   mg_opengl_create_pipeline;
        pipe.destroy_pipeline =   mg_opengl_destroy_pipeline;
        pipe.bind_pipeline    =   mg_opengl_bind_pipeline;

        pipe.create_vertex_buffer             =   mg_opengl_create_vertex_buffer;
        pipe.destroy_vertex_buffer            =   mg_opengl_destroy_vertex_buffer;

        pipe.create_index_buffer              =   mg_opengl_create_index_buffer;
        pipe.destroy_index_buffer             =   mg_opengl_destroy_index_buffer;

        pipe.create_dynamic_vertex_buffer     =   mg_opengl_create_dynamic_vertex_buffer;
        pipe.destroy_dynamic_vertex_buffer    =   mg_opengl_destroy_dynamic_vertex_buffer;
        pipe.update_dynamic_vertex_buffer     =   mg_opengl_update_dynamic_vertex_buffer;

        pipe.create_dynamic_index_buffer      =   mg_opengl_create_dynamic_index_buffer;
        pipe.destroy_dynamic_index_buffer     =   mg_opengl_destroy_dynamic_index_buffer;
        pipe.update_dynamic_index_buffer      =   mg_opengl_update_dynamic_index_buffer;

        pipe.bind_vertex_buffer               =   mg_opengl_bind_vertex_buffer;
        pipe.bind_dynamic_vertex_buffer       =   mg_opengl_bind_dynamic_vertex_buffer;
        pipe.bind_index_buffer                =   mg_opengl_bind_index_buffer;
        pipe.bind_dynamic_index_buffer        =   mg_opengl_bind_dynamic_index_buffer;

        pipe.bind_uniforms                =   mg_opengl_renderer_bind_uniforms;

        pipe.create_image     =   mg_opengl_create_image;
        pipe.destroy_image    =   mg_opengl_destroy_image;
        pipe.update_image     =   mg_opengl_update_image;
        //pipe.bind_image       =   mg_opengl_bind_image;

        pipe.create_sampler     =   mg_opengl_create_sampler;
        pipe.destroy_sampler    =   mg_opengl_destroy_sampler;

        pipe.create_image_array = mg_opengl_create_image_array;
        pipe.destroy_image_array = mg_opengl_destroy_image_array;
        pipe.update_image_array = mg_opengl_update_image_array;
        pipe.bind_image_array = mg_opengl_bind_image_array;

        pipe.create_framebuffer   =   mg_opengl_create_framebuffer;
        pipe.destroy_framebuffer  =   mg_opengl_destroy_framebuffer;

        pipe.draw         =   mg_opengl_renderer_draw;
        pipe.draw_indexed =   mg_opengl_renderer_draw_indexed;
        break;
#if MG_PLATFORM_WINDOWS
    case MG_RENDERER_TYPE_DIRECT3D11:
        pipe.initialize       =   mg_d3d11_renderer_initialize;
        pipe.shutdown         =   mg_d3d11_renderer_shutdown;
        pipe.begin      =   mg_d3d11_renderer_begin;
        pipe.end        =   mg_d3d11_renderer_end;

        pipe.wait             =   mg_d3d11_renderer_wait;
        pipe.viewport         =   mg_d3d11_renderer_viewport;
        pipe.scissor          =   mg_d3d11_renderer_scissor;

        pipe.configure_swapchain    =   mg_d3d11_renderer_configure_swapchain;

        pipe.create_render_pass           =   mg_d3d11_create_render_pass;
        pipe.destroy_render_pass          =   mg_d3d11_destroy_render_pass;
        pipe.begin_render_pass            =   mg_d3d11_begin_render_pass;
        pipe.begin_default_render_pass    =   mg_d3d11_begin_default_render_pass;
        pipe.end_render_pass              =   mg_d3d11_end_render_pass;

        pipe.create_pipeline  =   mg_d3d11_create_pipeline;
        pipe.destroy_pipeline =   mg_d3d11_destroy_pipeline;
        pipe.bind_pipeline    =   mg_d3d11_bind_pipeline;

        pipe.create_vertex_buffer             =   mg_d3d11_create_vertex_buffer;
        pipe.destroy_vertex_buffer            =   mg_d3d11_destroy_buffer;

        pipe.create_index_buffer              =   mg_d3d11_create_index_buffer;
        pipe.destroy_index_buffer             =   mg_d3d11_destroy_buffer;

        pipe.create_dynamic_vertex_buffer     =   mg_d3d11_create_dynamic_vertex_buffer;
        pipe.destroy_dynamic_vertex_buffer    =   mg_d3d11_destroy_buffer;
        pipe.update_dynamic_vertex_buffer     =   mg_d3d11_update_dynamic_buffer;

        pipe.create_dynamic_index_buffer      =   mg_d3d11_create_dynamic_index_buffer;
        pipe.destroy_dynamic_index_buffer     =   mg_d3d11_destroy_buffer;
        pipe.update_dynamic_index_buffer      =   mg_d3d11_update_dynamic_buffer;

        pipe.bind_vertex_buffer               =   mg_d3d11_bind_vertex_buffer;
        pipe.bind_dynamic_vertex_buffer       =   mg_d3d11_bind_vertex_buffer;
        pipe.bind_index_buffer                =   mg_d3d11_bind_index_buffer;
        pipe.bind_dynamic_index_buffer        =   mg_d3d11_bind_index_buffer;
        
        pipe.bind_uniforms                =   mg_d3d11_renderer_bind_uniforms;

        pipe.create_image     =   mg_d3d11_create_image;
        pipe.destroy_image    =   mg_d3d11_destroy_image;
        pipe.update_image     =   mg_d3d11_update_image;

        pipe.create_sampler     =   mg_d3d11_create_sampler;
        pipe.destroy_sampler    =   mg_d3d11_destroy_sampler;

        pipe.create_image_array = mg_d3d11_create_image_array;
        pipe.destroy_image_array = mg_d3d11_destroy_image_array;
        pipe.update_image_array = mg_d3d11_update_image_array;
        pipe.bind_image_array = mg_d3d11_bind_image_array;

        pipe.create_framebuffer   =   mg_d3d11_create_framebuffer;
        pipe.destroy_framebuffer  =   mg_d3d11_destroy_framebuffer;

        pipe.draw           =   mg_d3d11_renderer_draw;
        pipe.draw_indexed   =   mg_d3d11_renderer_draw_indexed;
        break;
#endif
    }

    pipe.type = init_info->type;
    pipe.initialize(init_info);
}

void mgfx_shutdown(void)
{
    pipe.shutdown();
}

void mgfx_begin(void)
{
    pipe.begin();
}

void mgfx_end(void)
{
    pipe.end();
}

void mgfx_wait(void)
{
    pipe.wait();
}

void mgfx_viewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    pipe.viewport(x, y, width, height);
}

void mgfx_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    pipe.scissor(x, y, width, height);
}

void mgfx_configure_swapchain(mg_swapchain_config_info *config_info)
{
    pipe.configure_swapchain(config_info);
}

mg_render_pass mgfx_create_render_pass(mg_render_pass_create_info *create_info)
{
    mg_render_pass render_pass;
    render_pass = pipe.create_render_pass(create_info);
    return render_pass;
}

void mgfx_destroy_render_pass(mg_render_pass render_pass)
{
    pipe.destroy_render_pass(render_pass);
}

void mgfx_begin_render_pass(mg_render_pass render_pass, mg_framebuffer framebuffer, mg_render_pass_begin_info *begin_info)
{
    pipe.begin_render_pass(render_pass, framebuffer, begin_info);
}

void mgfx_begin_default_render_pass(mg_render_pass_begin_info *begin_info)
{
    pipe.begin_default_render_pass(begin_info);
}

void mgfx_end_render_pass(void)
{
    pipe.end_render_pass();
}

void mgfx_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    pipe.draw(vertex_count, first_vertex);
}

void mgfx_draw_indexed(uint32_t index_count, uint32_t first_index, uint32_t vertex_offset)
{
    pipe.draw_indexed(index_count, first_index, vertex_offset);
}

mg_pipeline mgfx_create_pipeline(mg_pipeline_create_info *create_info)
{
    mg_pipeline pipeline;
    pipeline = pipe.create_pipeline(create_info);
    return pipeline;
}

void mgfx_destroy_pipeline(mg_pipeline pipeline)
{
    pipe.destroy_pipeline(pipeline);
}

void mgfx_bind_pipeline(mg_pipeline pipeline)
{
    pipe.bind_pipeline(pipeline);
}

mg_vertex_buffer mgfx_create_vertex_buffer(size_t size, void *data)
{
    mg_vertex_buffer buffer;
    buffer = pipe.create_vertex_buffer(size, data);
    return buffer;
}

void mgfx_destroy_vertex_buffer(mg_vertex_buffer buffer)
{
    pipe.destroy_vertex_buffer(buffer);
}

mg_index_buffer mgfx_create_index_buffer(size_t size, void *data)
{
    mg_index_buffer buffer;
    buffer = pipe.create_index_buffer(size, data);
    return buffer;
}

void mgfx_destroy_index_buffer(mg_index_buffer buffer)
{
    pipe.destroy_index_buffer(buffer);
}

mg_dynamic_vertex_buffer mgfx_create_dynamic_vertex_buffer(size_t size)
{
    mg_dynamic_vertex_buffer buffer;
    buffer = pipe.create_dynamic_vertex_buffer(size);
    return buffer;
}

void mgfx_destroy_dynamic_vertex_buffer(mg_dynamic_vertex_buffer buffer)
{
    pipe.destroy_dynamic_vertex_buffer(buffer);
}

void mgfx_update_dynamic_vertex_buffer(mg_dynamic_vertex_buffer buffer, size_t size, void *data)
{
    pipe.update_dynamic_vertex_buffer(buffer, size, data);
}

mg_dynamic_index_buffer mgfx_create_dynamic_index_buffer(size_t size)
{
    mg_dynamic_index_buffer buffer;
    buffer = pipe.create_dynamic_index_buffer(size);
    return buffer;
}

void mgfx_destroy_dynamic_index_buffer(mg_dynamic_index_buffer buffer)
{
    pipe.destroy_dynamic_index_buffer(buffer);
}

void mgfx_update_dynamic_index_buffer(mg_dynamic_index_buffer buffer, size_t size, void *data)
{
    pipe.update_dynamic_index_buffer(buffer, size, data);
}

void mgfx_bind_vertex_buffer(mg_vertex_buffer buffer)
{
    pipe.bind_vertex_buffer(buffer);
}

void mgfx_bind_dynamic_vertex_buffer(mg_dynamic_vertex_buffer buffer)
{
    pipe.bind_dynamic_vertex_buffer(buffer);
}

void mgfx_bind_index_buffer(mg_index_buffer buffer, mg_index_type index_type)
{
    pipe.bind_index_buffer(buffer, index_type);
}

void mgfx_bind_dynamic_index_buffer(mg_index_buffer buffer, mg_index_type index_type)
{
    pipe.bind_dynamic_index_buffer(buffer, index_type);
}

void mgfx_bind_uniforms(uint32_t binding, size_t size, void *data)
{
    pipe.bind_uniforms(binding, size, data);
}

mg_image mgfx_create_image(mg_image_create_info *create_info)
{
    mg_image image;
    image = pipe.create_image(create_info);
    return image;
}

void mgfx_destroy_image(mg_image image)
{
    pipe.destroy_image(image);
}

void mgfx_update_image(mg_image image, mg_image_write_info *write_info)
{
    pipe.update_image(image, write_info);
}

mg_image_array mgfx_create_image_array(void)
{
    mg_image_array array = pipe.create_image_array();
    return array;
}

void mgfx_destroy_image_array(mg_image_array array)
{
    pipe.destroy_image_array(array);
}

void mgfx_update_image_array(mg_image_array array, mg_image *images, mg_sampler *samplers, uint32_t count)
{
    pipe.update_image_array(array, images, samplers, count);
}

void mgfx_bind_image_array(mg_image_array array)
{
    pipe.bind_image_array(array);
}

mg_sampler mgfx_create_sampler(mg_sampler_create_info *create_info)
{
    mg_sampler sampler;
    sampler = pipe.create_sampler(create_info);
    return sampler;
}

mg_framebuffer mgfx_create_framebuffer(mg_framebuffer_create_info *create_info)
{
    mg_framebuffer framebuffer;
    framebuffer = pipe.create_framebuffer(create_info);
    return framebuffer;
}

void mgfx_destroy_framebuffer(mg_framebuffer framebuffer)
{
    pipe.destroy_framebuffer(framebuffer);
}

void mgfx_destroy_sampler(mg_sampler sampler)
{
    pipe.destroy_sampler(sampler);
}

mg_renderer_type mgfx_get_type(void)
{
    return pipe.type;
}