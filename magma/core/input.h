#pragma once

#include "base.h"

typedef enum mg_mouse_buttons
{
    MG_MOUSE_BUTTON_LEFT,
    MG_MOUSE_BUTTON_RIGHT,
    MG_MOUSE_BUTTON_MIDDLE,
    MG_MOUSE_BUTTON_MAX_BUTTONS
}
mg_mouse_buttons_t;

typedef enum mg_keys
{
    MG_KEYBACKSPACE = 0x08,
    MG_KEYENTER = 0x0D,
    MG_KEYTAB = 0x09,
    MG_KEYSHIFT = 0x10,
    MG_KEYCONTROL = 0x11,

    MG_KEYPAUSE = 0x13,
    MG_KEYCAPITAL = 0x14,

    MG_KEYESCAPE = 0x1B,

    MG_KEYCONVERT = 0x1C,
    MG_KEYNONCONVERT = 0x1D,
    MG_KEYACCEPT = 0x1E,
    MG_KEYMODECHANGE = 0x1F,

    MG_KEYSPACE = 0x20,
    MG_KEYPAGEUP = 0x21,
    MG_KEYPAGEDOWN = 0x22,
    MG_KEYEND = 0x23,
    MG_KEYHOME = 0x24,
    MG_KEYLEFT = 0x25,
    MG_KEYUP = 0x26,
    MG_KEYRIGHT = 0x27,
    MG_KEYDOWN = 0x28,
    MG_KEYSELECT = 0x29,
    MG_KEYPRINT = 0x2A,
    MG_KEYEXECUTE = 0x2B,
    MG_KEYPRINTSCREEN = 0x2C,
    MG_KEYINSERT = 0x2D,
    MG_KEYDELETE = 0x2E,
    MG_KEYHELP = 0x2F,

    MG_KEY0 = 0x30,
    MG_KEY1 = 0x31,
    MG_KEY2 = 0x32,
    MG_KEY3 = 0x33,
    MG_KEY4 = 0x34,
    MG_KEY5 = 0x35,
    MG_KEY6 = 0x36,
    MG_KEY7 = 0x37,
    MG_KEY8 = 0x38,
    MG_KEY9 = 0x39,

    MG_KEYA = 0x41,
    MG_KEYB = 0x42,
    MG_KEYC = 0x43,
    MG_KEYD = 0x44,
    MG_KEYE = 0x45,
    MG_KEYF = 0x46,
    MG_KEYG = 0x47,
    MG_KEYH = 0x48,
    MG_KEYI = 0x49,
    MG_KEYJ = 0x4A,
    MG_KEYK = 0x4B,
    MG_KEYL = 0x4C,
    MG_KEYM = 0x4D,
    MG_KEYN = 0x4E,
    MG_KEYO = 0x4F,
    MG_KEYP = 0x50,
    MG_KEYQ = 0x51,
    MG_KEYR = 0x52,
    MG_KEYS = 0x53,
    MG_KEYT = 0x54,
    MG_KEYU = 0x55,
    MG_KEYV = 0x56,
    MG_KEYW = 0x57,
    MG_KEYX = 0x58,
    MG_KEYY = 0x59,
    MG_KEYZ = 0x5A,

    MG_KEYLSUPER = 0x5B,
    MG_KEYRSUPER = 0x5C,
    MG_KEYAPPS = 0x5D,

    MG_KEYSLEEP = 0x5F,

    MG_KEYNUMPAD0 = 0x60,
    MG_KEYNUMPAD1 = 0x61,
    MG_KEYNUMPAD2 = 0x62,
    MG_KEYNUMPAD3 = 0x63,
    MG_KEYNUMPAD4 = 0x64,
    MG_KEYNUMPAD5 = 0x65,
    MG_KEYNUMPAD6 = 0x66,
    MG_KEYNUMPAD7 = 0x67,
    MG_KEYNUMPAD8 = 0x68,
    MG_KEYNUMPAD9 = 0x69,
    MG_KEYMULTIPLY = 0x6A,
    MG_KEYADD = 0x6B,
    MG_KEYSEPARATOR = 0x6C,
    MG_KEYSUBTRACT = 0x6D,
    MG_KEYDECIMAL = 0x6E,
    MG_KEYDIVIDE = 0x6F,

    MG_KEYF1 = 0x70,
    MG_KEYF2 = 0x71,
    MG_KEYF3 = 0x72,
    MG_KEYF4 = 0x73,
    MG_KEYF5 = 0x74,
    MG_KEYF6 = 0x75,
    MG_KEYF7 = 0x76,
    MG_KEYF8 = 0x77,
    MG_KEYF9 = 0x78,
    MG_KEYF10 = 0x79,
    MG_KEYF11 = 0x7A,
    MG_KEYF12 = 0x7B,
    MG_KEYF13 = 0x7C,
    MG_KEYF14 = 0x7D,
    MG_KEYF15 = 0x7E,
    MG_KEYF16 = 0x7F,
    MG_KEYF17 = 0x80,
    MG_KEYF18 = 0x81,
    MG_KEYF19 = 0x82,
    MG_KEYF20 = 0x83,
    MG_KEYF21 = 0x84,
    MG_KEYF22 = 0x85,
    MG_KEYF23 = 0x86,
    MG_KEYF24 = 0x87,

    MG_KEYNUMLOCK = 0x90,

    MG_KEYSCROLL = 0x91,

    MG_KEYNUMPAD_EQUAL = 0x92,

    MG_KEYLSHIFT = 0xA0,
    MG_KEYRSHIFT = 0xA1,
    MG_KEYLCONTROL = 0xA2,
    MG_KEYRCONTROL = 0xA3,
    MG_KEYLALT = 0xA4,
    MG_KEYRALT = 0xA5,

    MG_KEYSEMICOLON = 0x3B,

    MG_KEYAPOSTROPHE = 0xDE,
    MG_KEYQUOTE = MG_KEYAPOSTROPHE,
    MG_KEYEQUAL = 0xBB,
    MG_KEYCOMMA = 0xBC,
    MG_KEYMINUS = 0xBD,
    MG_KEYPERIOD = 0xBE,
    MG_KEYSLASH = 0xBF,

    MG_KEYGRAVE = 0xC0,

    MG_KEYLBRACKET = 0xDB,
    MG_KEYPIPE = 0xDC,
    MG_KEYBACKSLASH = MG_KEYPIPE,
    MG_KEYRBRACKET = 0xDD,

    KEYS_MAX_KEYS = 0xFF
}
mg_keys_t;

void mg_input_process_key           (mg_keys_t key, bool pressed);
void mg_input_process_mouse_button  (mg_mouse_buttons_t button, bool pressed);
void mg_input_process_mouse_move    (int16_t x, int16_t y);
void mg_input_process_mouse_wheel   (int8_t z_delta);

MG_API bool     mg_input_is_key_down            (mg_keys_t key);
MG_API bool     mg_input_is_mouse_button_down   (mg_mouse_buttons_t button);

MG_API int8_t   mg_input_get_mouse_delta        (void);
MG_API void     mg_input_get_mouse_position     (int32_t *x, int32_t *y);