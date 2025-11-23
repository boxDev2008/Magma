#include <magma/core/event.h>
#include <magma/core/event_types.h>

#include <magma/core/app.h>
#include <magma/rendering/renderer.h>

void on_resize(mg_resized_event_data *data)
{
    if (data->width == 0 || data->height == 0)
        return;

    mgfx_reset(data->width, data->height, true);
}

void on_initialize(int32_t argc, char* const* argv, uint32_t width, uint32_t height)
{
    mg_event_connect(MG_EVENT_CODE_RESIZED, (mg_event)on_resize);
    
    mgfx_init_info renderer_init_info = {
        .type = MG_RENDERER_TYPE_OPENGL,
        .width = width,
        .height = height,
        .vsync = true
    };
    
    mgfx_initialize(&renderer_init_info);
}

void on_shutdown(void)
{
	mgfx_shutdown();
}

void on_update(float delta_time)
{
    mgfx_begin();
	mgfx_end();
}

static const mg_app_init_info app_info = {
    .name = "Sandbox",
    .flags = MG_PLATFORM_FLAG_RESIZABLE,
    .width = 1280,
    .height = 720,
    .events = {
        .initialize = on_initialize,
        .shutdown = on_shutdown,
        .update = on_update
    }
};
MG_APP_DEFINE_ENTRY(app_info);