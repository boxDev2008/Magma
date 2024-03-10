#include "event.h"

static mg_event_t registered_events[MG_MAX_EVENT_CODE];

void mg_event_connect(mg_system_event_code_t code, PFN_on_event on_event)
{
    registered_events[code].callback = on_event;
}

void mg_event_call(mg_system_event_code_t code, void *data)
{
    if (registered_events[code].callback)
        registered_events[code].callback(data);
}