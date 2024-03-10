#include <magma/core/event.h>
#include <magma/core/event_types.h>

#include <magma/platform/platform.h>

#include <magma/rendering/rhi_renderer.h>

static bool is_running = true;

void on_application_quit(mg_application_quit_event_data_t *data)
{
    is_running = false;
}

void on_resize(mg_resized_event_data_t *data)
{
    if (data->width == 0 || data->height == 0)
        return;

    mg_swapchain_config_info_t config_info = {
        .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
        .present_mode = MG_PRESENT_MODE_FIFO,
        .width = data->width,
        .height = data->height
    };

    mg_rhi_renderer_configure_swapchain(&config_info);
}

int main(void)
{
    mg_platform_init_info_t platform_init_info = {
        .title = "Magma Sandbox",
        .position_x = 200,
        .position_y = 100,
        .width = 1280,
        .height = 720
    };

    mg_event_connect(MG_EVENT_CODE_APPLICATION_QUIT, (PFN_on_event)on_application_quit);

    mg_platform_t *platform = mg_platform_initialize(&platform_init_info);
    mg_event_connect(MG_EVENT_CODE_RESIZED, (PFN_on_event)on_resize);

    mg_swapchain_config_info_t swapchain_config = {
        .format = MG_PIXEL_FORMAT_R8G8B8A8_SRGB,
        .present_mode = MG_PRESENT_MODE_FIFO,
        .width = platform_init_info.width,
        .height = platform_init_info.height
    };

    mg_renderer_init_info_t renderer_init_info = {
        .platform = platform,
        .type = MG_RENDERER_TYPE_VULKAN,
        .swapchain_config_info = &swapchain_config
    };

    mg_rhi_renderer_initialize(&renderer_init_info);

    while (is_running)
    {
        mg_rhi_renderer_begin_frame();

        mg_rhi_renderer_end_frame();
        mg_rhi_renderer_present_frame();

        mg_platform_poll_messages(platform);
    }

    mg_rhi_renderer_shutdown();
    mg_platform_shutdown(platform);

    return 0;
}