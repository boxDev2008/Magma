#include "app.h"
#include "event.h"
#include "event_types.h"
#include "platform/platform.h"

#ifdef MG_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

static bool app_running;
static mg_app_init_info* app_info;

static struct {
    float last, current, delta;
} app_time;

static void mg_platform_on_quit(mg_quit_event_data *data)
{
    app_running = false;
#ifdef MG_PLATFORM_EMSCRIPTEN
    emscripten_cancel_main_loop();
#endif
}

static void mg_app_main_loop(void)
{
    mg_platform_poll_events();

    app_time.current = mg_get_time();
    app_time.delta = app_time.current - app_time.last;

    app_info->events.update(app_time.delta);

    app_time.last = app_time.current;
}

void mg_app_run(mg_app_init_info *info, int32_t argc, char* const* argv)
{
    mg_platform_init_info init_info;
    init_info.title = info->name;
    init_info.flags = info->flags;
#if MG_PLATFORM_EMSCRIPTEN
    double w, h;
    emscripten_get_element_css_size("#canvas", &w, &h);
    init_info.width = (int)w;
    init_info.height = (int)h;
#else
    init_info.width = info->width;
    init_info.height = info->height;
#endif
    mg_platform_initialize(&init_info);

    mg_event_connect(MG_EVENT_CODE_QUIT, mg_platform_on_quit);

    info->events.initialize(argc, argv, init_info.width, init_info.height);

    app_info = info;
    app_running = true;
    app_time.last = mg_get_time();

#ifdef MG_PLATFORM_EMSCRIPTEN
    emscripten_set_main_loop(mg_app_main_loop, 0, 1);
#else
    while (app_running)
        mg_app_main_loop();
    app_info->events.shutdown();
    mg_platform_shutdown();
#endif
}

void mg_app_close(void)
{
    app_running = false;
}