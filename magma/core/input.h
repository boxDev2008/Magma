#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mg_mouse_buttons
{
    MG_MOUSE_BUTTON_LEFT,
    MG_MOUSE_BUTTON_RIGHT,
    MG_MOUSE_BUTTON_MIDDLE,
    MG_MOUSE_BUTTON_MAX_BUTTONS
}
mg_mouse_buttons;

typedef enum mg_keys
{
    MG_KEY_BACKSPACE = 0x08,
    MG_KEY_ENTER = 0x0D,
    MG_KEY_TAB = 0x09,
    MG_KEY_SHIFT = 0x10,
    MG_KEY_CONTROL = 0x11,

    MG_KEY_PAUSE = 0x13,
    MG_KEY_CAPITAL = 0x14,

    MG_KEY_ESCAPE = 0x1B,

    MG_KEY_CONVERT = 0x1C,
    MG_KEY_NONCONVERT = 0x1D,
    MG_KEY_ACCEPT = 0x1E,
    MG_KEY_MODECHANGE = 0x1F,

    MG_KEY_SPACE = 0x20,
    MG_KEY_PAGE_UP = 0x21,
    MG_KEY_PAGE_DOWN = 0x22,
    MG_KEY_END = 0x23,
    MG_KEY_HOME = 0x24,
    MG_KEY_LEFT = 0x25,
    MG_KEY_UP = 0x26,
    MG_KEY_RIGHT = 0x27,
    MG_KEY_DOWN = 0x28,
    MG_KEY_SELECT = 0x29,
    MG_KEY_PRINT = 0x2A,
    MG_KEY_EXECUTE = 0x2B,
    MG_KEY_PRINT_SCREEN = 0x2C,
    MG_KEY_INSERT = 0x2D,
    MG_KEY_DELETE = 0x2E,
    MG_KEY_HELP = 0x2F,

    MG_KEY_0 = 0x30,
    MG_KEY_1 = 0x31,
    MG_KEY_2 = 0x32,
    MG_KEY_3 = 0x33,
    MG_KEY_4 = 0x34,
    MG_KEY_5 = 0x35,
    MG_KEY_6 = 0x36,
    MG_KEY_7 = 0x37,
    MG_KEY_8 = 0x38,
    MG_KEY_9 = 0x39,

    MG_KEY_A = 0x41,
    MG_KEY_B = 0x42,
    MG_KEY_C = 0x43,
    MG_KEY_D = 0x44,
    MG_KEY_E = 0x45,
    MG_KEY_F = 0x46,
    MG_KEY_G = 0x47,
    MG_KEY_H = 0x48,
    MG_KEY_I = 0x49,
    MG_KEY_J = 0x4A,
    MG_KEY_K = 0x4B,
    MG_KEY_L = 0x4C,
    MG_KEY_M = 0x4D,
    MG_KEY_N = 0x4E,
    MG_KEY_O = 0x4F,
    MG_KEY_P = 0x50,
    MG_KEY_Q = 0x51,
    MG_KEY_R = 0x52,
    MG_KEY_S = 0x53,
    MG_KEY_T = 0x54,
    MG_KEY_U = 0x55,
    MG_KEY_V = 0x56,
    MG_KEY_W = 0x57,
    MG_KEY_X = 0x58,
    MG_KEY_Y = 0x59,
    MG_KEY_Z = 0x5A,

    MG_KEY_LSUPER = 0x5B,
    MG_KEY_RSUPER = 0x5C,
    MG_KEY_APPS = 0x5D,

    MG_KEY_SLEEP = 0x5F,

    MG_KEY_NUMPAD0 = 0x60,
    MG_KEY_NUMPAD1 = 0x61,
    MG_KEY_NUMPAD2 = 0x62,
    MG_KEY_NUMPAD3 = 0x63,
    MG_KEY_NUMPAD4 = 0x64,
    MG_KEY_NUMPAD5 = 0x65,
    MG_KEY_NUMPAD6 = 0x66,
    MG_KEY_NUMPAD7 = 0x67,
    MG_KEY_NUMPAD8 = 0x68,
    MG_KEY_NUMPAD9 = 0x69,
    MG_KEY_MULTIPLY = 0x6A,
    MG_KEY_ADD = 0x6B,
    MG_KEY_SEPARATOR = 0x6C,
    MG_KEY_SUBTRACT = 0x6D,
    MG_KEY_DECIMAL = 0x6E,
    MG_KEY_DIVIDE = 0x6F,

    MG_KEY_F1 = 0x70,
    MG_KEY_F2 = 0x71,
    MG_KEY_F3 = 0x72,
    MG_KEY_F4 = 0x73,
    MG_KEY_F5 = 0x74,
    MG_KEY_F6 = 0x75,
    MG_KEY_F7 = 0x76,
    MG_KEY_F8 = 0x77,
    MG_KEY_F9 = 0x78,
    MG_KEY_F10 = 0x79,
    MG_KEY_F11 = 0x7A,
    MG_KEY_F12 = 0x7B,
    MG_KEY_F13 = 0x7C,
    MG_KEY_F14 = 0x7D,
    MG_KEY_F15 = 0x7E,
    MG_KEY_F16 = 0x7F,
    MG_KEY_F17 = 0x80,
    MG_KEY_F18 = 0x81,
    MG_KEY_F19 = 0x82,
    MG_KEY_F20 = 0x83,
    MG_KEY_F21 = 0x84,
    MG_KEY_F22 = 0x85,
    MG_KEY_F23 = 0x86,
    MG_KEY_F24 = 0x87,

    MG_KEY_NUMLOCK = 0x90,

    MG_KEY_SCROLL = 0x91,

    MG_KEY_NUMPAD_EQUAL = 0x92,

    MG_KEY_LSHIFT = 0xA0,
    MG_KEY_RSHIFT = 0xA1,
    MG_KEY_LCONTROL = 0xA2,
    MG_KEY_RCONTROL = 0xA3,
    MG_KEY_LALT = 0xA4,
    MG_KEY_RALT = 0xA5,

    MG_KEY_SEMICOLON = 0x3B,

    MG_KEY_APOSTROPHE = 0xDE,
    MG_KEY_QUOTE = MG_KEY_APOSTROPHE,
    MG_KEY_EQUAL = 0xBB,
    MG_KEY_COMMA = 0xBC,
    MG_KEY_MINUS = 0xBD,
    MG_KEY_PERIOD = 0xBE,
    MG_KEY_SLASH = 0xBF,

    MG_KEY_GRAVE = 0xC0,

    MG_KEY_LBRACKET = 0xDB,
    MG_KEY_PIPE = 0xDC,
    MG_KEY_BACKSLASH = MG_KEY_PIPE,
    MG_KEY_RBRACKET = 0xDD,

    MG_KEY_MAX_KEYS = 0xFF
}
mg_keys;

typedef enum mg_gamepad_buttons
{
    MG_GAMEPAD_BUTTON_DPAD_UP,
    MG_GAMEPAD_BUTTON_DPAD_DOWN,
    MG_GAMEPAD_BUTTON_DPAD_LEFT,
    MG_GAMEPAD_BUTTON_DPAD_RIGHT,
    MG_GAMEPAD_BUTTON_START,
    MG_GAMEPAD_BUTTON_BACK,
    MG_GAMEPAD_BUTTON_LEFT_THUMB,
    MG_GAMEPAD_BUTTON_RIGHT_THUMB,
    MG_GAMEPAD_BUTTON_LEFT_SHOULDER,
    MG_GAMEPAD_BUTTON_RIGHT_SHOULDER,
    MG_GAMEPAD_BUTTON_A,
    MG_GAMEPAD_BUTTON_B,
    MG_GAMEPAD_BUTTON_X,
    MG_GAMEPAD_BUTTON_Y
}
mg_gamepad_buttons;

typedef enum mg_gamepad_triggers
{
    MG_GAMEPAD_LEFT_TRIGGER,
    MG_GAMEPAD_RIGHT_TRIGGER
}
mg_gamepad_triggers;

typedef struct mg_gamepad_state
{
    float left_thumbstick_x, left_thumbstick_y;
    float right_thumbstick_x, right_thumbstick_y;
    float left_trigger;
    float right_trigger;
    bool buttons[14];
}
mg_gamepad_state;

void mg_input_process_key           (mg_keys key, bool pressed);
void mg_input_process_char          (char ch);

void mg_input_process_mouse_button  (mg_mouse_buttons button, bool pressed);
void mg_input_process_mouse_move    (int32_t x, int32_t y);
void mg_input_process_mouse_wheel   (int8_t z_delta);

MG_API bool     mg_input_is_key_down            (mg_keys key);
MG_API bool     mg_input_is_mouse_button_down   (mg_mouse_buttons button);

MG_API int8_t   mg_input_get_mouse_delta        (void);
MG_API void     mg_input_get_mouse_position     (int32_t *x, int32_t *y);

MG_API mg_gamepad_state mg_input_get_gamepad_state  (uint8_t index);

#ifdef __cplusplus
}
#endif