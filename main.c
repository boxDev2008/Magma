#include "magma/core/event.h"
#include "magma/core/event_types.h"

#include "magma/platform/platform.h"

#include "magma/rendering/renderer.h"

static b8 is_running = true;

void on_application_quit(mg_application_quit_event_data_t *data)
{
    is_running = false;
}

void on_resize(mg_resized_event_data_t *data)
{
    
}

int main(void)
{
    mg_platform_t plat_state;
    mg_window_description_t win_description = {
        .title = "Hello uwu!",
        .width = 800,
        .height = 600,
        .position_x = 200,
        .position_y = 100
    };

    mg_event_register(MG_EVENT_CODE_APPLICATION_QUIT, (PFN_on_event)on_application_quit);
    mg_event_register(MG_EVENT_CODE_RESIZED, (PFN_on_event)on_resize);
    mg_platform_initialize(&plat_state, &win_description);
    mg_renderer_initialize(&plat_state, MG_RENDERER_TYPE_OPENGL);

    while (is_running)
    {
        mg_platform_poll_messages(&plat_state);
    }

    mg_renderer_shutdown();
    mg_platform_shutdown(&plat_state);

    return 0;
}