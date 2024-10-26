#include "event.h"

static mg_event registered_events[MG_MAX_EVENT_CODE];

void mg_event_connect(mg_system_event_code code, mg_event on_event)
{
    registered_events[code] = on_event;
}

void mg_event_call(mg_system_event_code code, void *data)
{
    if (registered_events[code])
        registered_events[code](data);
}