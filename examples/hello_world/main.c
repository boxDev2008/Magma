#include <magma/core/event.h>
#include <magma/core/event_types.h>

#include <magma/core/app.h>
#include <magma/rendering/renderer.h>

void on_resize(mg_resized_event_data *data)
{
    if (data->width == 0 || data->height == 0)
        return;

    mg_swapchain_config_info config_info = {
        .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
        .vsync = true,
        .width = data->width,
        .height = data->height
    };

    mgfx_configure_swapchain(&config_info);
}

void on_initialize(int32_t argc, char* const* argv, uint32_t width, uint32_t height)
{
    mg_event_connect(MG_EVENT_CODE_RESIZED, (mg_event)on_resize);

	mg_swapchain_config_info swapchain_config = {
        .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
        .vsync = true,
        .width = width,
        .height = height
    };
    
    mgfx_init_info renderer_init_info = {
        .type = MG_RENDERER_TYPE_OPENGL,
        .swapchain_config_info = &swapchain_config
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
    .name = "Hello, Magma!",
    .width = 1280,
    .height = 720,
    .events = {
        .initialize = on_initialize,
        .shutdown = on_shutdown,
        .update = on_update
    }
};
MG_APP_DEFINE_ENTRY(app_info);