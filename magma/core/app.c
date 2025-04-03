#include "app.h"
#include "event.h"
#include "event_types.h"

#include "platform/platform.h"

static bool app_running;

static void mg_platform_on_quit(mg_quit_event_data *data) { app_running = false; }

void mg_app_run(mg_app_init_info *info, int32_t argc, char* const* argv)
{
    mg_platform_init_info init_info;
    init_info.title = info->name;
    init_info.width = info->width;
    init_info.height = info->height;
    mg_platform *platform = mg_platform_initialize(&init_info);

    mg_event_connect(MG_EVENT_CODE_QUIT, mg_platform_on_quit);

    info->events.initialize(argc, argv, init_info.width, init_info.height, platform);

    struct
    {
        float last, current, delta;
    }
    app_time = { 0 };

    app_running = true;
    while (app_running)
    {
        mg_platform_poll_events(platform);

        app_time.current = mg_get_time();
        app_time.delta = app_time.current - app_time.last;
        info->events.update(app_time.delta);
        app_time.last = app_time.current;
    }

    info->events.shutdown();
    mg_platform_shutdown(platform);
}