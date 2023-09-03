#include "llapi_renderer.h"

#include "vulkan/vulkan_renderer.h"
#include "vulkan/vulkan_program.h"
#include "vulkan/vulkan_buffer.h"
#include "vulkan/vulkan_image.h"
#include "vulkan/vulkan_descriptor_set.h"

typedef struct mg_renderer_plugin mg_renderer_plugin_t;
struct mg_renderer_plugin
{
    mg_platform_t *platform;

    void (*initialize)      (mg_platform_t *platform);
    void (*shutdown)        (void);

    void (*resize)          (int32_t width, int32_t height);

    void (*begin_frame)     (void);
    void (*end_frame)       (void);

    void (*present_frame)   (void);
    void (*wait)            (void);

    void *(*create_descriptor_set_layout)   (mg_descriptor_set_layout_create_info_t *create_info);
    void (*destroy_descriptor_set_layout)   (void *internal_data);

    void *(*create_descriptor_set)          (mg_descriptor_set_create_info_t *create_info);
    void (*update_descriptor_set)           (void *internal_data, mg_descriptor_write_t *descriptor_write);
    void (*destroy_descriptor_set)          (void *internal_data);

    void (*bind_descriptor_set)             (void *internal_data, void *program_internal_data, uint32_t set_index);

    void *(*create_program) (mg_program_create_info_t *create_info);
    void (*destroy_program) (void *internal_data);
    void (*bind_program)    (void *internal_data);

    void *(*create_buffer)              (mg_buffer_create_info_t *create_info);
    void (*update_buffer)               (void *internal_data, mg_buffer_update_info_t *update_data);
    void (*destroy_buffer)              (void *internal_data);

    void *(*create_texture_image)       (mg_texture_image_create_info_t *create_info);
    void (*destroy_texture_image)       (void *internal_data);

    void *(*create_sampler)       (mg_sampler_create_info_t *create_info);
    void (*destroy_sampler)       (void *internal_data);

    void (*bind_vertex_buffer)          (void *internal_data);
    void (*bind_index_buffer)           (void *internal_data, mg_index_type_t index_type);

    void (*draw)            (uint32_t vertex_count, uint32_t first_vertex);
    void (*draw_indexed)    (uint32_t index_count, uint32_t first_index);

    void (*push_constants)  (void *program, uint32_t size, void *data);
};

mg_renderer_plugin_t plugin;

void mg_llapi_renderer_initialize(mg_platform_t *platform, mg_renderer_type_t type)
{
    switch (type)
    {
        case MG_RENDERER_TYPE_OPENGL:
            //plugin.initialize   =   mg_opengl_renderer_initialize;
            //plugin.shutdown     =   mg_opengl_renderer_shutdown;
        break;
        case MG_RENDERER_TYPE_VULKAN:
            plugin.initialize       =   mg_vulkan_renderer_initialize;
            plugin.shutdown         =   mg_vulkan_renderer_shutdown;
            plugin.resize           =   mg_vulkan_renderer_resize;
            plugin.begin_frame      =   mg_vulkan_renderer_begin_frame;
            plugin.end_frame        =   mg_vulkan_renderer_end_frame;
            plugin.present_frame    =   mg_vulkan_renderer_present;
            plugin.wait             =   mg_vulkan_renderer_wait;

            plugin.create_descriptor_set_layout     =   mg_vulkan_create_descriptor_set_layout;
            plugin.destroy_descriptor_set_layout    =   mg_vulkan_destroy_descriptor_set_layout;

            plugin.create_descriptor_set    =   mg_vulkan_create_descriptor_set;
            plugin.update_descriptor_set    =   mg_vulkan_update_descriptor_set;
            plugin.destroy_descriptor_set   =   mg_vulkan_destroy_descriptor_set;

            plugin.bind_descriptor_set  =   mg_vulkan_bind_descriptor_set;

            plugin.create_program   =   mg_vulkan_create_program;
            plugin.destroy_program  =   mg_vulkan_destroy_program;
            plugin.bind_program     =   mg_vulkan_bind_program;

            plugin.create_buffer            =   mg_vulkan_create_buffer;
            plugin.update_buffer            =   mg_vulkan_update_buffer;
            plugin.destroy_buffer           =   mg_vulkan_destroy_buffer;

            plugin.create_texture_image     =   mg_vulkan_create_texture_image;
            plugin.destroy_texture_image    =   mg_vulkan_destroy_texture_image;

            plugin.create_sampler     =   mg_vulkan_create_sampler;
            plugin.destroy_sampler    =   mg_vulkan_destroy_sampler;

            plugin.bind_vertex_buffer   =   mg_vulkan_bind_vertex_buffer;
            plugin.bind_index_buffer    =   mg_vulkan_bind_index_buffer;

            plugin.draw         =   mg_vulkan_renderer_draw;
            plugin.draw_indexed =   mg_vulkan_renderer_draw_indexed;

            plugin.push_constants   =   mg_vulkan_renderer_push_constants;
        break;
    }

    plugin.initialize(platform);
}

void mg_llapi_renderer_shutdown(void)
{
    plugin.shutdown();
}

void mg_llapi_renderer_resize(int32_t width, int32_t height)
{
    plugin.resize(width, height);
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

void mg_llapi_bind_descriptor_set(mg_descriptor_set_t descriptor_set, mg_program_t program, uint32_t set_index)
{
    plugin.bind_descriptor_set(descriptor_set.internal_data, program.internal_data, set_index);
}

mg_program_t mg_llapi_renderer_create_program(mg_program_create_info_t *create_info)
{
    mg_program_t program;
    program.internal_data = plugin.create_program(create_info);
    return program;
}

void mg_llapi_renderer_destroy_program(mg_program_t program)
{
    plugin.destroy_program(program.internal_data);
}

void mg_llapi_renderer_bind_program(mg_program_t program)
{
    plugin.bind_program(program.internal_data);
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

void mg_llapi_renderer_destroy_texture_image(mg_texture_image_t texture_image)
{
    plugin.destroy_texture_image(texture_image.internal_data);
}

mg_sampler_t mg_llapi_renderer_create_sampler(mg_sampler_create_info_t *create_info)
{
    mg_sampler_t sampler;
    sampler.internal_data = plugin.create_sampler(create_info);
    return sampler;
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

void mg_llapi_renderer_push_constants(mg_program_t program, uint32_t size, void *data)
{
    plugin.push_constants(program.internal_data, size, data);
}