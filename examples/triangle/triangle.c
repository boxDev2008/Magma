#define MG_IMPL
#include "mgapp.h"

#define MGFX_OPENGL
#include "mgfx.h"

#include "triangle.glsl.h"

static mgfx_pipeline pip;
static mgfx_buffer vb;

void on_start(void)
{
    mgfx_init(&(mgfx_init_info){
        .primary_handle = mgapp_primary_handle(),
        .secondary_handle = mgapp_secondary_handle(),
        .width = mgapp_width(),
        .height = mgapp_height(),
        .vsync = true
    });

    vb = mgfx_create_buffer(&(mgfx_buffer_create_info){
        .usage = MGFX_BUFFER_USAGE_VERTEX,
        .data = (float[]){
            0.0f, 0.5f, 1.0f, 0.5f, 0.5f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.5f,
            -0.5f, -0.5f, 0.5f, 0.5f, 1.0f,
            0.0f, 0.5f, 1.0f, 0.5f, 0.5f
        },
        .size = 20 * sizeof(float)
    });

    pip = mgfx_create_pipeline(&(mgfx_pipeline_create_info){
        .vertex_layout = {
            MGFX_VERTEX_FORMAT_FLOAT2,
            MGFX_VERTEX_FORMAT_FLOAT3
        },
        .primitive_topology = MGFX_PRIMITIVE_TOPOLOGY_LINE_STRIP,
        .shader = get_triangle_shader(mgfx_get_shader_lang())
    });
}

void on_end(void)
{
    mgfx_destroy_buffer(vb);
    mgfx_destroy_pipeline(pip);
    mgfx_shutdown();
}

void on_update(void)
{
    if (mgfx_begin() != MGFX_RESULT_SUCCESS)
        return;

    mgfx_bind_pass(&(mgfx_pass_info){.clear = {0.05f, 0.05f, 0.05f, 1.0f}});
    mgfx_bind_pipeline(pip);
    mgfx_bind_vertex_buffer(vb);
    mgfx_draw(4, 0);

    mgfx_end();
}

void on_event(const mgapp_event *event)
{
    if (event->type == MGAPP_EVENT_RESIZE)
        mgfx_resize(event->window.width, event->window.height);
}

mgapp_init_info mgapp_main(void)
{
    return (mgapp_init_info) {
        .title = "Triangle",
        .events = {
            .start = on_start,
            .end = on_end,
            .update = on_update,
            .event = on_event
        }
    };
}