#include "input.h"
#include "event.h"
#include "event_types.h"

typedef struct mg_keyboard_state
{
    bool keys[256];
}
mg_keyboard_state;

typedef struct mg_mouse_state
{
    int16_t x;
    int16_t y;
    int8_t z_delta;
    bool buttons[4];
}
mg_mouse_state;

/*typedef struct mg_gamepad_state
{
    uint32_t buttons;
    bool left_trigger;
    bool right_trigger;
    float thumb_left_x;
    float thumb_left_y;
    float thumb_right_x;
    float thumb_right_y;
}
mg_gamepad_state_t;*/

typedef struct mg_input_state
{
    mg_keyboard_state keyboard;
    mg_mouse_state mouse;
    //mg_gamepad_state_t gamepads[4];
}
mg_input_state;

static mg_input_state input_state;

void mg_input_process_key(mg_keys key, bool pressed)
{
    input_state.keyboard.keys[key] = pressed;
    mg_key_event_data data = {key};
    mg_event_call(pressed ? MG_EVENT_CODE_KEY_PRESSED : MG_EVENT_CODE_KEY_RELEASED, (void*)&data);
}

void mg_input_process_char(char ch)
{
    mg_char_event_data data = { ch };
    mg_event_call(MG_EVENT_CODE_CHAR, (void*)&data);
}

void mg_input_process_mouse_button(mg_mouse_buttons button, bool pressed)
{
    input_state.mouse.buttons[button] = pressed;
    mg_mouse_button_event_data data = {button};
    mg_event_call(pressed ? MG_EVENT_CODE_MOUSE_BUTTON_PRESSED : MG_EVENT_CODE_MOUSE_BUTTON_RELEASED, (void*)&data);
}

void mg_input_process_mouse_move(int16_t x, int16_t y)
{
    input_state.mouse.x = x;
    input_state.mouse.y = y;
    mg_mouse_moved_event_data data = {x, y};
    mg_event_call(MG_EVENT_CODE_MOUSE_MOVED, (void*)&data);
}

void mg_input_process_mouse_wheel(int8_t z_delta)
{
    input_state.mouse.z_delta = z_delta;
    mg_mouse_wheel_event_data data = {z_delta};
    mg_event_call(MG_EVENT_CODE_MOUSE_WHEEL, (void*)&data);
}

bool mg_input_is_key_down(mg_keys key)
{
    return input_state.keyboard.keys[key];
}

bool mg_input_is_mouse_button_down(mg_mouse_buttons button)
{
    return input_state.mouse.buttons[button];
}

int8_t mg_input_get_mouse_delta(void)
{
    return input_state.mouse.z_delta;
}

void mg_input_get_mouse_position(int32_t *x, int32_t *y)
{
    *x = input_state.mouse.x;
    *y = input_state.mouse.y;
}