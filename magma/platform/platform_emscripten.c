#include "platform/platform.h"

#if MG_PLATFORM_EMSCRIPTEN

#include "core/input.h"
#include "core/event.h"
#include "core/event_types.h"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

EM_BOOL key_down_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
{
    mg_input_process_key(e->keyCode, true);
    return EM_TRUE;
}

EM_BOOL key_up_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
{
    mg_input_process_key(e->keyCode, false);
    return EM_TRUE;
}

EM_BOOL mouse_down_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
    mg_input_process_mouse_button(e->button, true);
    return EM_TRUE;
}

EM_BOOL mouse_up_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
    mg_input_process_mouse_button(e->button, false);
    return EM_TRUE;
}

EM_BOOL mouse_move_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
    mg_input_process_mouse_move(e->clientX, e->clientY);
    return EM_TRUE;
}

EM_BOOL mouse_wheel_callback(int eventType, const EmscriptenWheelEvent *e, void *userData)
{
    mg_input_process_mouse_wheel(e->deltaY);
    return EM_TRUE;
}

EM_BOOL resize_callback(int eventType, const EmscriptenUiEvent *e, void *userData)
{
    mg_resized_event_data data = {e->windowInnerWidth, e->windowInnerHeight};
    mg_event_call(MG_EVENT_CODE_RESIZED, (void*)&data);
    return EM_TRUE;
}

EM_BOOL quit_callback(int eventType, const void *reserved, void *userData)
{
    mg_application_quit_event_data data = {NULL};
    mg_event_call(MG_EVENT_CODE_QUIT, (void*)&data);
    return EM_TRUE;
}

mg_platform *mg_platform_initialize(mg_platform_init_info *init_info)
{
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, key_down_callback);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, key_up_callback);
    emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, mouse_down_callback);
    emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, mouse_up_callback);
    emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, mouse_move_callback);
    emscripten_set_wheel_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, mouse_wheel_callback);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_TRUE, resize_callback);
    emscripten_set_beforeunload_callback(NULL, quit_callback);

    return NULL;
}

void mg_platform_shutdown(mg_platform *platform)
{

}

void mg_platform_poll_events(mg_platform *platform)
{

}

double mg_get_time(void)
{

}

#endif