#include "input.h"
#include "event.h"
#include "event_types.h"

typedef struct mg_keyboard_state
{
    b8 keys[256];
}
mg_keyboard_state_t;

typedef struct mg_mouse_state
{
    i16 x;
    i16 y;
    i8 z_delta;
    b8 buttons[4];
}
mg_mouse_state_t;

typedef struct mg_mg_input_state
{
    mg_keyboard_state_t keyboard;
    mg_mouse_state_t mouse;
}
mg_mg_input_state_t;

static mg_mg_input_state_t mg_input_state;

void mg_input_process_key(mg_keys_t key, b8 pressed)
{
    mg_input_state.keyboard.keys[key] = pressed;
    mg_key_event_data_t data = {key};
    mg_event_call(pressed ? MG_EVENT_CODE_KEY_PRESSED : MG_EVENT_CODE_KEY_RELEASED, (void*)&data);
}

void mg_input_process_mouse_button(mg_mouse_buttons_t button, b8 pressed)
{
    mg_input_state.mouse.buttons[button] = pressed;
    mg_mouse_button_event_data_t data = {button};
    mg_event_call(pressed ? MG_EVENT_CODE_MOUSE_BUTTON_PRESSED : MG_EVENT_CODE_MOUSE_BUTTON_RELEASED, (void*)&data);
}

void mg_input_process_mouse_move(i16 x, i16 y)
{
    mg_input_state.mouse.x = x;
    mg_input_state.mouse.y = y;
    mg_mouse_moved_event_data_t data = {x, y};
    mg_event_call(MG_EVENT_CODE_MOUSE_MOVED, (void*)&data);
}

void mg_input_process_mouse_wheel(i8 z_delta)
{
    mg_input_state.mouse.z_delta = z_delta;
    mg_mouse_wheel_event_data_t data = {z_delta};
    mg_event_call(MG_EVENT_CODE_MOUSE_WHEEL, (void*)&data);
}

b8 mg_input_is_key_down(mg_keys_t key)
{
    return mg_input_state.keyboard.keys[key];
}

b8 mg_input_is_mouse_button_down(mg_mouse_buttons_t button)
{
    return mg_input_state.mouse.buttons[button];
}

b8 mg_input_get_mouse_delta(void)
{
    return mg_input_state.mouse.z_delta;
}

void mg_input_get_mouse_position(i32 *x, i32 *y)
{
    *x = mg_input_state.mouse.x;
    *y = mg_input_state.mouse.y;
}