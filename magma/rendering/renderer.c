#include "renderer.h"

#include "backends/opengl/opengl_renderer.h"
#include "backends/opengl/opengl_buffer.h"
#include "backends/opengl/opengl_pipeline.h"
#include "backends/opengl/opengl_image.h"
#include "backends/opengl/opengl_render_pass.h"

#if !MG_PLATFORM_EMSCRIPTEN
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

typedef void (*mgfx_initialize_fn)(const mgfx_init_info *init_info);
typedef void (*mgfx_shutdown_fn)(void);
typedef void (*mgfx_begin_fn)(void);
typedef void (*mgfx_end_fn)(void);
typedef void (*mgfx_present_frame_fn)(void);

typedef void (*mgfx_reset_fn)(uint32_t width, uint32_t height, bool vsync);

typedef void (*mgfx_viewport_fn)(int32_t x, int32_t y, uint32_t width, uint32_t height);
typedef void (*mgfx_scissor_fn)(int32_t x, int32_t y, uint32_t width, uint32_t height);

typedef void *(*mgfx_create_render_pass_fn)(const mg_render_pass_create_info *create_info);
typedef void (*mgfx_destroy_render_pass_fn)(void *render_pass);
typedef void (*mgfx_update_render_pass_fn)(void *render_pass, const mg_render_pass_update_info *update_info);
typedef void (*mgfx_bind_render_pass_fn)(void *render_pass, const mg_render_pass_bind_info *bind_info);

typedef void *(*mgfx_create_pipeline_fn)(const mg_pipeline_create_info *create_info);
typedef void (*mgfx_destroy_pipeline_fn)(void *pipeline);
typedef void (*mgfx_bind_pipeline_fn)(void *pipeline);

typedef void *(*mgfx_create_image_fn)(const mg_image_create_info *create_info);
typedef void (*mgfx_destroy_image_fn)(void *image);
typedef void (*mgfx_update_image_fn)(void *image, const mg_image_update_info *update_info);
typedef void (*mgfx_bind_image_fn)(void *image, void *sampler, uint32_t binding);

typedef void *(*mgfx_create_sampler_fn)(const mg_sampler_create_info *create_info);
typedef void (*mgfx_destroy_sampler_fn)(void *sampler);

typedef void *(*mgfx_create_buffer_fn)(const mg_buffer_create_info *create_info);
typedef void (*mgfx_destroy_buffer_fn)(void *buffer);
typedef void (*mgfx_update_buffer_fn)(void *buffer, size_t size, void *data);

typedef void (*mgfx_bind_vertex_buffer_fn)(void *buffer);
typedef void (*mgfx_bind_index_buffer_fn)(void *buffer, mg_index_type index_type);

typedef void (*mgfx_bind_uniforms_fn)(uint32_t binding, size_t size, void *data);

typedef void (*mgfx_draw_fn)(uint32_t vertex_count, uint32_t first_vertex);
typedef void (*mgfx_draw_indexed_fn)(uint32_t index_count, uint32_t first_index, int32_t vertex_offset);
typedef void (*mgfx_draw_instanced_fn)(uint32_t vertex_count, uint32_t first_vertex, uint32_t instance_count, uint32_t first_instance);
typedef void (*mgfx_draw_indexed_instanced_fn)(uint32_t index_count, uint32_t first_index, int32_t vertex_offset, uint32_t instance_count, uint32_t first_instance);
typedef void (*mgfx_dispatch_fn)(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);

typedef struct mgfx_pipe
{
    mg_platform *platform;
    mg_renderer_type type;

    mgfx_initialize_fn             initialize;
    mgfx_shutdown_fn               shutdown;

    mgfx_begin_fn                  begin;
    mgfx_end_fn                    end;
    mgfx_present_frame_fn          present_frame;

    mgfx_reset_fn                  reset;

    mgfx_viewport_fn               viewport;
    mgfx_scissor_fn                scissor;

    mgfx_create_render_pass_fn         create_render_pass;
    mgfx_destroy_render_pass_fn        destroy_render_pass;
    mgfx_update_render_pass_fn         update_render_pass;
    mgfx_bind_render_pass_fn          bind_render_pass;

    mgfx_create_pipeline_fn         create_pipeline;
    mgfx_destroy_pipeline_fn        destroy_pipeline;
    mgfx_bind_pipeline_fn           bind_pipeline;

    mgfx_create_image_fn            create_image;
    mgfx_destroy_image_fn           destroy_image;
    mgfx_update_image_fn            update_image;
    mgfx_bind_image_fn              bind_image;

    mgfx_create_sampler_fn          create_sampler;
    mgfx_destroy_sampler_fn         destroy_sampler;

    mgfx_create_buffer_fn           create_buffer;
    mgfx_destroy_buffer_fn          destroy_buffer;
    mgfx_update_buffer_fn           update_buffer;

    mgfx_bind_vertex_buffer_fn      bind_vertex_buffer;
    mgfx_bind_index_buffer_fn       bind_index_buffer;

    mgfx_bind_uniforms_fn           bind_uniforms;

    mgfx_draw_fn                    draw;
    mgfx_draw_indexed_fn            draw_indexed;
    mgfx_draw_instanced_fn          draw_instanced;
    mgfx_draw_indexed_instanced_fn  draw_indexed_instanced;
    mgfx_dispatch_fn                dispatch;
}
mgfx_pipe;

static mgfx_pipe pipe;

void mgfx_initialize(const mgfx_init_info *init_info)
{
    switch (init_info->type)
    {
#if !MG_PLATFORM_EMSCRIPTEN
    case MG_RENDERER_TYPE_VULKAN:
        pipe.initialize       =   mg_vulkan_renderer_initialize;
        pipe.shutdown         =   mg_vulkan_renderer_shutdown;
        pipe.begin      =   mg_vulkan_renderer_begin;
        pipe.end        =   mg_vulkan_renderer_end;
        pipe.viewport         =   mg_vulkan_renderer_viewport;
        pipe.scissor          =   mg_vulkan_renderer_scissor;

        pipe.reset          =   mg_vulkan_reset;

        pipe.create_render_pass           =   mg_vulkan_create_render_pass;
        pipe.destroy_render_pass          =   mg_vulkan_destroy_render_pass;
        pipe.update_render_pass           =   mg_vulkan_update_render_pass;
        pipe.bind_render_pass           =   mg_vulkan_bind_render_pass;

        pipe.create_pipeline   =   mg_vulkan_create_pipeline;
        pipe.destroy_pipeline  =   mg_vulkan_destroy_pipeline;
        pipe.bind_pipeline     =   mg_vulkan_bind_pipeline;

        pipe.create_buffer  =   mg_vulkan_create_buffer;
        pipe.destroy_buffer =   mg_vulkan_destroy_buffer;
        pipe.update_buffer  =   mg_vulkan_update_buffer;

        pipe.bind_vertex_buffer        =   mg_vulkan_bind_vertex_buffer;
        pipe.bind_index_buffer         =   mg_vulkan_bind_index_buffer;
        
        pipe.bind_uniforms                =   mg_vulkan_renderer_bind_uniforms;

        pipe.create_image     =   mg_vulkan_create_image;
        pipe.destroy_image    =   mg_vulkan_destroy_image;
        pipe.update_image     =   mg_vulkan_update_image;
        pipe.bind_image       =   mg_vulkan_bind_image;

        pipe.create_sampler     =   mg_vulkan_create_sampler;
        pipe.destroy_sampler    =   mg_vulkan_destroy_sampler;

        pipe.draw         =   mg_vulkan_renderer_draw;
        pipe.draw_indexed =   mg_vulkan_renderer_draw_indexed;
        pipe.draw_instanced = mg_vulkan_renderer_draw_instanced;
        pipe.draw_indexed_instanced = mg_vulkan_renderer_draw_indexed_instanced;
        pipe.dispatch     =   mg_vulkan_renderer_dispatch;
        break;
    case MG_RENDERER_TYPE_OPENGL:
#endif
    case MG_RENDERER_TYPE_OPENGLES:
        pipe.initialize       =   mg_opengl_renderer_initialize;
        pipe.shutdown         =   mg_opengl_renderer_shutdown;
        pipe.begin      =   mg_opengl_renderer_begin;
        pipe.end        =   mg_opengl_renderer_end;
        pipe.viewport         =   mg_opengl_renderer_viewport;
        pipe.scissor          =   mg_opengl_renderer_scissor;

        pipe.reset          =   mg_opengl_renderer_reset;

        pipe.create_render_pass         =   mg_opengl_create_render_pass;
        pipe.destroy_render_pass        =   mg_opengl_destroy_render_pass;
        pipe.update_render_pass         =   mg_opengl_update_render_pass;
        pipe.bind_render_pass           =   mg_opengl_bind_render_pass;

        pipe.create_pipeline  =   mg_opengl_create_pipeline;
        pipe.destroy_pipeline =   mg_opengl_destroy_pipeline;
        pipe.bind_pipeline    =   mg_opengl_bind_pipeline;

        pipe.create_buffer  =   mg_opengl_create_buffer;
        pipe.destroy_buffer =   mg_opengl_destroy_buffer;
        pipe.update_buffer  =   mg_opengl_update_buffer;

        pipe.bind_vertex_buffer        =   mg_opengl_bind_vertex_buffer;
        pipe.bind_index_buffer         =   mg_opengl_bind_index_buffer;

        pipe.bind_uniforms                =   mg_opengl_renderer_bind_uniforms;

        pipe.create_image     =   mg_opengl_create_image;
        pipe.destroy_image    =   mg_opengl_destroy_image;
        pipe.update_image     =   mg_opengl_update_image;
        pipe.bind_image       =   mg_opengl_bind_image;

        pipe.create_sampler     =   mg_opengl_create_sampler;
        pipe.destroy_sampler    =   mg_opengl_destroy_sampler;

        pipe.draw         =   mg_opengl_renderer_draw;
        pipe.draw_indexed =   mg_opengl_renderer_draw_indexed;
        pipe.draw_instanced = mg_opengl_renderer_draw_instanced;
        pipe.draw_indexed_instanced = mg_opengl_renderer_draw_indexed_instanced;
        pipe.dispatch     =   mg_opengl_renderer_dispatch;
        break;
#if MG_PLATFORM_WINDOWS
    case MG_RENDERER_TYPE_DIRECT3D11:
        pipe.initialize       =   mg_d3d11_renderer_initialize;
        pipe.shutdown         =   mg_d3d11_renderer_shutdown;
        pipe.begin      =   mg_d3d11_renderer_begin;
        pipe.end        =   mg_d3d11_renderer_end;

        pipe.viewport         =   mg_d3d11_renderer_viewport;
        pipe.scissor          =   mg_d3d11_renderer_scissor;

        pipe.reset    =   mg_d3d11_renderer_reset;

        pipe.create_render_pass           =   mg_d3d11_create_render_pass;
        pipe.destroy_render_pass          =   mg_d3d11_destroy_render_pass;
        pipe.update_render_pass           =   (mgfx_update_render_pass_fn)mg_d3d11_update_render_pass;
        pipe.bind_render_pass           =   mg_d3d11_bind_render_pass;

        pipe.create_pipeline  =   mg_d3d11_create_pipeline;
        pipe.destroy_pipeline =   mg_d3d11_destroy_pipeline;
        pipe.bind_pipeline    =   mg_d3d11_bind_pipeline;

        pipe.create_buffer  =   mg_d3d11_create_buffer;
        pipe.destroy_buffer =   mg_d3d11_destroy_buffer;
        pipe.update_buffer  =   mg_d3d11_update_buffer;

        pipe.bind_vertex_buffer        =   mg_d3d11_bind_vertex_buffer;
        pipe.bind_index_buffer         =   mg_d3d11_bind_index_buffer;
        
        pipe.bind_uniforms                =   mg_d3d11_renderer_bind_uniforms;

        pipe.create_image     =   mg_d3d11_create_image;
        pipe.destroy_image    =   mg_d3d11_destroy_image;
        pipe.update_image     =   mg_d3d11_update_image;
        pipe.bind_image       =   mg_d3d11_bind_image;

        pipe.create_sampler     =   mg_d3d11_create_sampler;
        pipe.destroy_sampler    =   mg_d3d11_destroy_sampler;

        pipe.draw           =   mg_d3d11_renderer_draw;
        pipe.draw_indexed   =   mg_d3d11_renderer_draw_indexed;
        pipe.draw_instanced = mg_d3d11_renderer_draw_instanced;
        pipe.draw_indexed_instanced = mg_d3d11_renderer_draw_indexed_instanced;
        pipe.dispatch       =   mg_d3d11_renderer_dispatch;
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

void mgfx_viewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    pipe.viewport(x, y, width, height);
}

void mgfx_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    pipe.scissor(x, y, width, height);
}

void mgfx_reset(uint32_t width, uint32_t height, bool vsync)
{
    pipe.reset(width, height, vsync);
}

mg_render_pass mgfx_create_render_pass(const mg_render_pass_create_info *create_info)
{
    mg_render_pass render_pass;
    render_pass = pipe.create_render_pass(create_info);
    return render_pass;
}

void mgfx_destroy_render_pass(mg_render_pass render_pass)
{
    pipe.destroy_render_pass(render_pass);
}

void mgfx_update_render_pass(mg_render_pass render_pass, const mg_render_pass_update_info *update_info)
{
    pipe.update_render_pass(render_pass, update_info);
}

void mgfx_bind_render_pass(mg_render_pass render_pass, const mg_render_pass_bind_info *begin_info)
{
    pipe.bind_render_pass(render_pass, begin_info);
}

void mgfx_draw(uint32_t vertex_count, uint32_t first_vertex)
{
    pipe.draw(vertex_count, first_vertex);
}

void mgfx_draw_indexed(uint32_t index_count, uint32_t first_index, uint32_t vertex_offset)
{
    pipe.draw_indexed(index_count, first_index, vertex_offset);
}

void mgfx_draw_instanced(uint32_t vertex_count, uint32_t first_vertex, uint32_t instance_count, uint32_t first_instance)
{
    pipe.draw_instanced(vertex_count, first_vertex, instance_count, first_instance);
}

void mgfx_draw_indexed_instanced(uint32_t index_count, uint32_t first_index, int32_t first_vertex, uint32_t instance_count, uint32_t first_instance)
{
    pipe.draw_indexed_instanced(index_count, first_index, first_vertex, instance_count, first_instance);
}

void mgfx_dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
{
    pipe.dispatch(group_count_x, group_count_y, group_count_z);
}

mg_pipeline mgfx_create_pipeline(const mg_pipeline_create_info *create_info)
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

mg_buffer mgfx_create_buffer(const mg_buffer_create_info *create_info)
{
    mg_buffer buffer;
    buffer = pipe.create_buffer(create_info);
    return buffer;
}

void mgfx_destroy_buffer(mg_buffer buffer)
{
    pipe.destroy_buffer(buffer);
}

void mgfx_update_buffer(mg_buffer buffer, size_t size, void *data)
{
    pipe.update_buffer(buffer, size, data);
}

void mgfx_bind_vertex_buffer(mg_buffer buffer)
{
    pipe.bind_vertex_buffer(buffer);
}

void mgfx_bind_index_buffer(mg_buffer buffer, mg_index_type index_type)
{
    pipe.bind_index_buffer(buffer, index_type);
}

void mgfx_bind_uniforms(uint32_t binding, size_t size, void *data)
{
    pipe.bind_uniforms(binding, size, data);
}

mg_image mgfx_create_image(const mg_image_create_info *create_info)
{
    mg_image image;
    image = pipe.create_image(create_info);
    return image;
}

void mgfx_destroy_image(mg_image image)
{
    pipe.destroy_image(image);
}

void mgfx_update_image(mg_image image, const mg_image_update_info *update_info)
{
    pipe.update_image(image, update_info);
}

void mgfx_bind_image(mg_image image, mg_sampler sampler, uint32_t binding)
{
    pipe.bind_image(image, sampler, binding);
}

mg_sampler mgfx_create_sampler(const mg_sampler_create_info *create_info)
{
    mg_sampler sampler;
    sampler = pipe.create_sampler(create_info);
    return sampler;
}

void mgfx_destroy_sampler(mg_sampler sampler)
{
    pipe.destroy_sampler(sampler);
}

mg_renderer_type mgfx_get_type(void)
{
    return pipe.type;
}