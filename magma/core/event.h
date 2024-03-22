#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PFN_on_event)(void *data);

typedef struct mg_event
{
    PFN_on_event callback;
}
mg_event_t;

typedef enum mg_system_event_code
{
    MG_EVENT_CODE_APPLICATION_QUIT,
    MG_EVENT_CODE_KEY_PRESSED,
    MG_EVENT_CODE_KEY_RELEASED,
    MG_EVENT_CODE_CHAR,
    MG_EVENT_CODE_MOUSE_BUTTON_PRESSED,
    MG_EVENT_CODE_MOUSE_BUTTON_RELEASED,
    MG_EVENT_CODE_MOUSE_MOVED,
    MG_EVENT_CODE_MOUSE_WHEEL,
    MG_EVENT_CODE_RESIZED,
    MG_MAX_EVENT_CODE = 0xFF
}
mg_system_event_code_t;

MG_API void mg_event_connect(mg_system_event_code_t code, PFN_on_event on_event);
MG_API void mg_event_call(mg_system_event_code_t code, void *data);

#ifdef __cplusplus
}
#endif