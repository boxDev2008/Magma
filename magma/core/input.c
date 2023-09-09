#include "input.h"
#include "event.h"
#include "event_types.h"

typedef struct mg_keyboard_state mg_keyboard_state_t;
struct mg_keyboard_state
{
    bool keys[256];
};

typedef struct mg_mouse_state mg_mouse_state_t;
struct mg_mouse_state
{
    int16_t x;
    int16_t y;
    int8_t z_delta;
    bool buttons[4];
};

typedef struct mg_mg_input_state mg_mg_input_state_t;
struct mg_mg_input_state
{
    mg_keyboard_state_t keyboard;
    mg_mouse_state_t mouse;
};

static mg_mg_input_state_t mg_input_state;

void mg_input_process_key(mg_keys_t key, bool pressed)
{
    mg_input_state.keyboard.keys[key] = pressed;
    mg_key_event_data_t data = {key};
    mg_event_call(pressed ? MG_EVENT_CODE_KEY_PRESSED : MG_EVENT_CODE_KEY_RELEASED, (void*)&data);
}

void mg_input_process_mouse_button(mg_mouse_buttons_t button, bool pressed)
{
    mg_input_state.mouse.buttons[button] = pressed;
    mg_mouse_button_event_data_t data = {button};
    mg_event_call(pressed ? MG_EVENT_CODE_MOUSE_BUTTON_PRESSED : MG_EVENT_CODE_MOUSE_BUTTON_RELEASED, (void*)&data);
}

void mg_input_process_mouse_move(int16_t x, int16_t y)
{
    mg_input_state.mouse.x = x;
    mg_input_state.mouse.y = y;
    mg_mouse_moved_event_data_t data = {x, y};
    mg_event_call(MG_EVENT_CODE_MOUSE_MOVED, (void*)&data);
}

void mg_input_process_mouse_wheel(int8_t z_delta)
{
    mg_input_state.mouse.z_delta = z_delta;
    mg_mouse_wheel_event_data_t data = {z_delta};
    mg_event_call(MG_EVENT_CODE_MOUSE_WHEEL, (void*)&data);
}

bool mg_input_is_key_down(mg_keys_t key)
{
    return mg_input_state.keyboard.keys[key];
}

bool mg_input_is_mouse_button_down(mg_mouse_buttons_t button)
{
    return mg_input_state.mouse.buttons[button];
}

int8_t mg_input_get_mouse_delta(void)
{
    return mg_input_state.mouse.z_delta;
}

void mg_input_get_mouse_position(int32_t *x, int32_t *y)
{
    *x = mg_input_state.mouse.x;
    *y = mg_input_state.mouse.y;
}