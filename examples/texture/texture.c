#define MG_IMPL
#include "mgapp.h"

#define MGFX_VULKAN
#include "mgfx.h"

#include "texture.glsl.h"

MG_DEFINE_APP("Texture")

static mgfx_pipeline pip;
static mgfx_buffer vb;
static mgfx_image image;
static mgfx_sampler sampler;

void mg_app_on_start(void)
{
    mgfx_init(&(mgfx_init_info){
        .primary_handle = mg_app_primary_handle(),
        .secondary_handle = mg_app_secondary_handle(),
        .width = mg_app_width(),
        .height = mg_app_height(),
        .vsync = true
    });
    vb = mgfx_create_buffer(&(mgfx_buffer_create_info){
        .usage = MGFX_BUFFER_USAGE_VERTEX,
        .data = (float[]){
            -0.5f, -0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 1.0f, 1.0f
        },
        .size = 16 * sizeof(float)
    });

    pip = mgfx_create_pipeline(&(mgfx_pipeline_create_info){
        .vertex_attributes = {
            MGFX_VERTEX_FORMAT_FLOAT2,
            MGFX_VERTEX_FORMAT_FLOAT2
        },
        .primitive_topology = MGFX_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
        .shader = get_texture_shader(mgfx_get_shader_lang())
    });

    image = mgfx_create_image(&(mgfx_image_create_info){
        .data = (void*)(uint32_t[]){
            0xFFFFFFFF, 0xFF000000,
            0xFF000000, 0xFFFFFFFF
        },
        .width = 2,
        .height = 2,
        .format = MGFX_FORMAT_RGBA8_UNORM
    });

    sampler = mgfx_create_sampler(&(mgfx_sampler_create_info){
        .min_filter = MGFX_SAMPLER_FILTER_NEAREST,
        .mag_filter = MGFX_SAMPLER_FILTER_NEAREST
    });
}

void mg_app_on_end(void)
{
    mgfx_destroy_sampler(sampler);
    mgfx_destroy_image(image);
    mgfx_destroy_buffer(vb);
    mgfx_destroy_pipeline(pip);
    mgfx_shutdown();
}

void mg_app_on_update(void)
{
    if (mgfx_begin() != MGFX_RESULT_SUCCESS)
        return;

    mgfx_bind_pass(&(mgfx_pass_info){.clear = {0.05f, 0.05f, 0.05f, 1.0f}});
    mgfx_bind_pipeline(pip);
    mgfx_bind_image(image, sampler, 0);
    mgfx_bind_vertex_buffer(vb);
    mgfx_draw(4, 0);

    mgfx_end();
}

void mg_app_on_event(const mg_app_event *event)
{
    if (event->type == MG_APP_EVENT_RESIZE)
        mgfx_resize(event->window_width, event->window_height);
}