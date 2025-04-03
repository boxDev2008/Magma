#include <magma/core/event.h>
#include <magma/core/event_types.h>

#include <magma/core/app.h>
#include <magma/rendering/renderer.h>

#include <magma/core/log.h>

void on_resize(mg_resized_event_data *data)
{
    if (data->width == 0 || data->height == 0)
        return;

    mg_swapchain_config_info config_info = {
        .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
        .present_mode = MG_PRESENT_MODE_FIFO,
        .width = data->width,
        .height = data->height
    };

    mgfx_configure_swapchain(&config_info);
	MG_LOG_WARNING("Window resized to %d x %d", data->width, data->height);
}

void on_initialize(int32_t argc, char* const* argv, uint32_t width, uint32_t height, mg_platform *platform)
{
    mg_event_connect(MG_EVENT_CODE_RESIZED, (mg_event)on_resize);

	mg_swapchain_config_info swapchain_config = {
        .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
        .present_mode = MG_PRESENT_MODE_FIFO,
        .width = width,
        .height = height
    };
    
    mgfx_init_info renderer_init_info = {
        .platform = platform,
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
	mgfx_begin_default_render_pass(&(mg_render_pass_begin_info){
		.clear_value = (mg_vec4){0.0f, 0.0f, 1.0f, 1.0f},
		.render_area = (mg_vec4){0.0f, 0.0f, 1280.0f, 720.0f}
	});
	mgfx_end_render_pass();
	mgfx_end();
}

MG_APP_DEFINE_ENTRY({
    .name = "Sandbox",
    .width = 1280,
    .height = 720,
    .events = {
        .initialize = on_initialize,
        .shutdown = on_shutdown,
        .update = on_update
    }
});