#define MG_IMPL
#include "mgapp.h"

#define MGFX_D3D11
#include "mgfx.h"

#include "triangle.glsl.h"

MG_DEFINE_APP("Sandbox")

static mgfx_pipeline pip;
static mgfx_buffer vb;

void mg_app_on_start(void)
{
    mgfx_init(&(mgfx_init_info){
        .handle = mg_app_handle(),
        .width = mg_app_width(),
        .height = mg_app_height(),
        .vsync = true
    });
    vb = mgfx_create_buffer(&(mgfx_buffer_create_info){
        .usage = MGFX_BUFFER_USAGE_VERTEX,
        .data = (float[]){
            0.0f, -0.5f, 1.0f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.5f,
            -0.5f, 0.5f, 0.5f, 0.5f, 1.0f,
            0.0f, -0.5f, 1.0f, 0.5f, 0.5f
        },
        .size = 20 * sizeof(float)
    });

    pip = mgfx_create_pipeline(&(mgfx_pipeline_create_info){
        .vertex_attributes = {
            MGFX_VERTEX_FORMAT_FLOAT2,
            MGFX_VERTEX_FORMAT_FLOAT3
        },
        .primitive_topology = MGFX_PRIMITIVE_TOPOLOGY_LINE_STRIP,
        .shader = get_triangle_shader(mgfx_get_shader_lang()),
    });
}

void mg_app_on_end(void)
{
    mgfx_destroy_buffer(vb);
    mgfx_destroy_pipeline(pip);
    mgfx_shutdown();
}

void mg_app_on_update(void)
{
    mgfx_begin();
    mgfx_bind_render_pass(0, (mgfx_color){0.05f, 0.05f, 0.05f, 1.0f});
    mgfx_bind_pipeline(pip);
    mgfx_bind_vertex_buffer(vb);
    mgfx_draw(4, 0);
    mgfx_end();
}

void mg_app_on_event(const mg_app_event *event)
{
    if (event->type == MG_APP_EVENT_RESIZE)
        mgfx_resize(event->window_width, event->window_height);
}