#pragma once

#include "base.h"

#include "platform/platform.h"
#include "input.h"

typedef struct mg_application_quit_event_data
{
    mg_platform_t *platform;
}
mg_application_quit_event_data_t;

typedef struct mg_resized_event_data
{
    uint32_t width, height;
}
mg_resized_event_data_t;

typedef struct mg_mouse_moved_event_data
{
    int32_t x, y;
}
mg_mouse_moved_event_data_t;

typedef struct mg_mouse_wheel_event_data
{
    int8_t z_delta;
}
mg_mouse_wheel_event_data_t;

typedef struct mg_keyevent_data
{
    mg_keys_t key;
}
mg_key_event_data_t;

typedef struct mg_mouse_button_event_data
{
    mg_mouse_buttons_t button;
}
mg_mouse_button_event_data_t;