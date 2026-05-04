#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef MG_APP_API
#if defined(_WIN32) && defined(MG_DLL) && defined(MG_APP_IMPL)
#define MG_APP_API __declspec(dllexport)
#elif defined(_WIN32) && defined(MG_DLL)
#define MG_APP_API __declspec(dllimport)
#else
#define MG_APP_API extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t mg_mouse_button;
enum 
{
    MG_MOUSE_BUTTON_LEFT,
    MG_MOUSE_BUTTON_RIGHT,
    MG_MOUSE_BUTTON_MIDDLE,
    MG_MOUSE_BUTTON_MAX
};

typedef uint8_t mg_key;
enum
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

    MG_KEY_MAX = 0xFF
};

typedef uint8_t mg_app_event_type;
enum
{
    MG_APP_EVENT_RESIZE,
    MG_APP_EVENT_KEY_DOWN,
    MG_APP_EVENT_KEY_UP,
    MG_APP_EVENT_CHAR,
    MG_APP_EVENT_MOUSE_DOWN,
    MG_APP_EVENT_MOUSE_UP,
    MG_APP_EVENT_MOUSE_MOVE,
    MG_APP_EVENT_MOUSE_SCROLL
};

typedef struct
{
    union
    {
        struct
        {
            int32_t window_width, window_height;
        };
        struct
        {
            int32_t mouse_x, mouse_y;
        };
        int8_t scroll_delta;
        mg_key key;
        mg_mouse_button mouse_button;
        char ch;
    };
    
    mg_app_event_type type;
}
mg_app_event;

typedef struct
{
    const char *title;
    uint32_t flags;
    uint32_t width, height;
    struct
    {
        void (*start)(void);
        void (*end)(void);
        void (*update)(void);
        void (*event)(const mg_app_event *event);
    }
    events;
}
mg_app_init_info;

MG_APP_API int32_t mg_app_run(const mg_app_init_info *info);
MG_APP_API void mg_app_close(void);
MG_APP_API float mg_app_time(void);
MG_APP_API float mg_app_delta_time(void);
MG_APP_API int32_t mg_app_width(void);
MG_APP_API int32_t mg_app_height(void);

MG_APP_API bool mg_app_key_down(mg_key key);
MG_APP_API bool mg_app_key_pressed(mg_key key);
MG_APP_API bool mg_app_mouse_down(mg_mouse_button button);
MG_APP_API bool mg_app_mouse_clicked(mg_mouse_button button);
MG_APP_API int8_t mg_app_mouse_scroll_delta(void);
MG_APP_API int32_t mg_app_mouse_x(void);
MG_APP_API int32_t mg_app_mouse_y(void);

MG_APP_API void *mg_app_handle(void);

#define MG_DEFINE_APP(name) \
int main(void) { \
    extern void mg_app_on_start(void); \
    extern void mg_app_on_end(void); \
    extern void mg_app_on_update(void); \
    extern void mg_app_on_event(const mg_app_event *event); \
    mg_app_init_info info = { \
        .title = name, \
        .events = { \
            .start  = mg_app_on_start, \
            .end    = mg_app_on_end, \
            .update = mg_app_on_update, \
            .event  = mg_app_on_event \
        } \
    }; \
    return mg_app_run(&info); \
}

#if defined(MG_IMPL) || defined(MGAPP_IMPL)

#include <string.h>

typedef struct
{
    struct
    {
        bool keys[256];
        bool keys_pressed[256];
    }
    keyboard;
    struct
    {
        int16_t x;
        int16_t y;
        int8_t delta;
        bool buttons[4];
        bool buttons_pressed[4];   
    }
    mouse;
}
mg_app_input_state;

static mg_app_input_state input_state;

static inline void mg_app_input_process_key(mg_key key, bool pressed)
{
    if (pressed && !input_state.keyboard.keys[key])
        input_state.keyboard.keys_pressed[key] = true;
    input_state.keyboard.keys[key] = pressed;
}

static inline void mg_app_input_process_mouse_button(mg_mouse_button button, bool pressed)
{
    if (pressed && !input_state.mouse.buttons[button])
        input_state.mouse.buttons_pressed[button] = true;
    input_state.mouse.buttons[button] = pressed;
}

static inline void mg_app_input_reset(void)
{
    input_state.mouse.delta = 0;
    memset(input_state.keyboard.keys_pressed, 0, sizeof(input_state.keyboard.keys_pressed));
    memset(input_state.mouse.buttons_pressed, 0, sizeof(input_state.mouse.buttons_pressed));
}

bool mg_app_key_down(mg_key key)
{
    return input_state.keyboard.keys[key];
}

bool mg_app_key_pressed(mg_key key)
{
    return input_state.keyboard.keys_pressed[key];
}

bool mg_app_mouse_down(mg_mouse_button button)
{
    return input_state.mouse.buttons[button];
}

bool mg_app_mouse_clicked(mg_mouse_button button)
{
    return input_state.mouse.buttons_pressed[button];
}

int8_t mg_app_mouse_scroll_delta(void)
{
    return input_state.mouse.delta;
}

int32_t mg_app_mouse_x(void)
{
    return input_state.mouse.x;
}

int32_t mg_app_mouse_y(void)
{
    return input_state.mouse.y;
}

#if defined(__EMSCRIPTEN__)

#include <emscripten.h>
#include <emscripten/html5.h>

typedef struct mg_emscripten_platform
{
    int32_t window_width, window_height;
    void (*on_event_call)(const mg_app_event *event);
    float time, delta_time;
    bool running;
    const mg_app_init_info *info;
}
mg_emscripten_platform;

static mg_emscripten_platform platform;

static inline void mg_app_call_event(const mg_app_event *event)
{
    if (platform.on_event_call)
        platform.on_event_call(event);
}

static EM_BOOL mg_em_resize_callback(int event_type, const EmscriptenUiEvent *ui_event, void *user_data)
{
    double w, h;
    emscripten_get_element_css_size("#canvas", &w, &h);

    platform.window_width  = (int32_t)w;
    platform.window_height = (int32_t)h;

    mg_app_call_event(&(mg_app_event){
        .window_width  = platform.window_width,
        .window_height = platform.window_height,
        .type          = MG_APP_EVENT_RESIZE
    });

    return EM_TRUE;
}

static void mg_em_frame(void *user_data)
{
    double now = emscripten_get_now() * 0.001;
    platform.delta_time = (float)(now - platform.time);
    platform.time = (float)now;

    if (platform.info->events.update)
        platform.info->events.update();

    mg_app_input_reset();
}

int32_t mg_app_run(const mg_app_init_info *info)
{
    platform.info = info;
    platform.on_event_call = info->events.event;
    platform.running = true;

    double w, h;
    emscripten_get_element_css_size("#canvas", &w, &h);
    platform.window_width  = (int32_t)w;
    platform.window_height = (int32_t)h;

    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_TRUE, mg_em_resize_callback);

    platform.time = (float)(emscripten_get_now() * 0.001);
    platform.delta_time = 0.0f;

    if (info->events.start)
        info->events.start();

    emscripten_set_main_loop_arg(mg_em_frame, NULL, 0, 1);

    return 0;
}

void mg_app_close(void)
{
    emscripten_cancel_main_loop();
    platform.running = false;

    if (platform.info && platform.info->events.end)
        platform.info->events.end();
}

float mg_app_time(void)
{
    return platform.time;
}

float mg_app_delta_time(void)
{
    return platform.delta_time;
}

int32_t mg_app_width(void)
{
    return platform.window_width;
}

int32_t mg_app_height(void)
{
    return platform.window_height;
}

void *mg_app_handle(void)
{
    return (void*)&platform;
}

#elif defined(_WIN32)

#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "user32")

typedef struct mg_win32_platform
{
    HWND hwnd;
    HINSTANCE instace;
    int32_t window_width, window_height;

    void (*on_event_call)(const mg_app_event *event);
    float time, delta_time;
}
mg_win32_platform;

static mg_win32_platform platform;

static float clock_frequency;
static LARGE_INTEGER start_time;

static inline void mg_app_call_event(const mg_app_event *event)
{
    if (platform.on_event_call)
        platform.on_event_call(event);
}

static void mg_app_setup_clock(void)
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0f / (float)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);
}

static LRESULT CALLBACK mg_win32_process_message(HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
        {
            RECT r;
            GetClientRect(hwnd, &r);
            platform.window_width  = r.right - r.left;
            platform.window_height = r.bottom - r.top;

            mg_app_call_event(&(mg_app_event){
                .window_width = platform.window_width,
                .window_height = platform.window_height,
                .type = MG_APP_EVENT_RESIZE
            });
            break;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            mg_key key = (mg_key)w_param;
            mg_app_input_process_key(key, pressed);
            mg_app_call_event(&(mg_app_event){
                .key = key,
                .type = pressed ? MG_APP_EVENT_KEY_DOWN : MG_APP_EVENT_KEY_UP
            });
            break;
        }
        case WM_CHAR:
            mg_app_call_event(&(mg_app_event){
                .ch = (char)w_param,
                .type = MG_APP_EVENT_CHAR
            });
        break;
        case WM_MOUSEMOVE:
        {
            int32_t x = GET_X_LPARAM(l_param);
            int32_t y = GET_Y_LPARAM(l_param);
            input_state.mouse.x = x;
            input_state.mouse.y = y;
            mg_app_call_event(&(mg_app_event){
                .mouse_x = x,
                .mouse_y = y,
                .type = MG_APP_EVENT_MOUSE_MOVE
            });
            break;
        }
        case WM_MOUSEWHEEL:
        {
            int32_t delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (delta != 0)
            {
                delta = (delta < 0) ? -1 : 1;
                input_state.mouse.delta = (uint8_t)delta;
                mg_app_call_event(&(mg_app_event){
                    .scroll_delta = (uint8_t)delta,
                    .type = MG_APP_EVENT_MOUSE_SCROLL
                });
            }
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            bool pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            mg_mouse_button mouse_button = MG_MOUSE_BUTTON_MAX;
            switch (msg)
            {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    mouse_button = MG_MOUSE_BUTTON_LEFT;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    mouse_button = MG_MOUSE_BUTTON_MIDDLE;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    mouse_button = MG_MOUSE_BUTTON_RIGHT;
                    break;
            }

            mg_app_input_process_mouse_button(mouse_button, pressed);
            mg_app_call_event(&(mg_app_event){
                .mouse_button = mouse_button,
                .type = pressed ? MG_APP_EVENT_MOUSE_DOWN : MG_APP_EVENT_MOUSE_UP
            });
            break;
        }
    }
    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

int32_t mg_app_run(const mg_app_init_info *info)
{
    const char *CLASS_NAME = "MAGMA";
    platform.instace = GetModuleHandleA(NULL);

    mg_app_setup_clock();

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = mg_win32_process_message;
    wc.hInstance = platform.instace;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassA(&wc))
    {
        MessageBoxA(NULL, "Window registration failed!", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    const int32_t width = info->width ? info->width : CW_USEDEFAULT;
    const int32_t height = info->height ? info->height : CW_USEDEFAULT;

    platform.hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        info->title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        NULL, NULL,
        platform.instace,
        NULL
    );

    if (!platform.hwnd)
    {
        MessageBoxA(NULL, "Window creation failed!", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(platform.hwnd, SW_SHOW);
    UpdateWindow(platform.hwnd);

    platform.on_event_call = info->events.event;

    if (info->events.start)
        info->events.start();

    bool running = true;
    while (running)
    {
        MSG msg;
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        LARGE_INTEGER now_time;
        QueryPerformanceCounter(&now_time);
        float new_time = (float)(now_time.QuadPart - start_time.QuadPart) * clock_frequency;
        platform.delta_time = new_time - platform.time;
        platform.time = new_time;

        if (info->events.update)
            info->events.update();

        mg_app_input_reset();
    }

    if (info->events.end)
        info->events.end();

    DestroyWindow(platform.hwnd);
    UnregisterClassA(CLASS_NAME, platform.instace);
    return 0;
}

void mg_app_close(void)
{
    PostMessage(platform.hwnd, WM_CLOSE, 0, 0);
}

float mg_app_time(void)
{
    return platform.time;
}

float mg_app_delta_time(void)
{
    return platform.delta_time;
}

int32_t mg_app_width(void)
{
    return platform.window_width;
}

int32_t mg_app_height(void)
{
    return platform.window_height;
}

void *mg_app_handle(void)
{
    return (void*)&platform;
}

#elif defined(__linux__)
 
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <time.h>
#include <stdio.h>
 
typedef struct mg_xlib_platform
{
    Window window;
    Display *display;
    int screen;
    Atom wm_delete_window;
    void (*on_event_call)(const mg_app_event *event);
    int32_t window_width, window_height;
    float time, delta_time;
    bool running;
}
mg_xlib_platform;
 
static mg_xlib_platform platform;
 
static inline void mg_app_call_event(const mg_app_event *event)
{
    if (platform.on_event_call)
        platform.on_event_call(event);
}
 
static float mg_xlib_get_time(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (float)ts.tv_sec + (float)ts.tv_nsec * 1e-9f;
}

static mg_key mg_xlib_translate_key(KeySym sym)
{
    switch (sym)
    {
        case XK_a: case XK_A: return MG_KEY_A;
        case XK_b: case XK_B: return MG_KEY_B;
        case XK_c: case XK_C: return MG_KEY_C;
        case XK_d: case XK_D: return MG_KEY_D;
        case XK_e: case XK_E: return MG_KEY_E;
        case XK_f: case XK_F: return MG_KEY_F;
        case XK_g: case XK_G: return MG_KEY_G;
        case XK_h: case XK_H: return MG_KEY_H;
        case XK_i: case XK_I: return MG_KEY_I;
        case XK_j: case XK_J: return MG_KEY_J;
        case XK_k: case XK_K: return MG_KEY_K;
        case XK_l: case XK_L: return MG_KEY_L;
        case XK_m: case XK_M: return MG_KEY_M;
        case XK_n: case XK_N: return MG_KEY_N;
        case XK_o: case XK_O: return MG_KEY_O;
        case XK_p: case XK_P: return MG_KEY_P;
        case XK_q: case XK_Q: return MG_KEY_Q;
        case XK_r: case XK_R: return MG_KEY_R;
        case XK_s: case XK_S: return MG_KEY_S;
        case XK_t: case XK_T: return MG_KEY_T;
        case XK_u: case XK_U: return MG_KEY_U;
        case XK_v: case XK_V: return MG_KEY_V;
        case XK_w: case XK_W: return MG_KEY_W;
        case XK_x: case XK_X: return MG_KEY_X;
        case XK_y: case XK_Y: return MG_KEY_Y;
        case XK_z: case XK_Z: return MG_KEY_Z;

        case XK_0: return MG_KEY_0;
        case XK_1: return MG_KEY_1;
        case XK_2: return MG_KEY_2;
        case XK_3: return MG_KEY_3;
        case XK_4: return MG_KEY_4;
        case XK_5: return MG_KEY_5;
        case XK_6: return MG_KEY_6;
        case XK_7: return MG_KEY_7;
        case XK_8: return MG_KEY_8;
        case XK_9: return MG_KEY_9;

        case XK_F1:  return MG_KEY_F1;
        case XK_F2:  return MG_KEY_F2;
        case XK_F3:  return MG_KEY_F3;
        case XK_F4:  return MG_KEY_F4;
        case XK_F5:  return MG_KEY_F5;
        case XK_F6:  return MG_KEY_F6;
        case XK_F7:  return MG_KEY_F7;
        case XK_F8:  return MG_KEY_F8;
        case XK_F9:  return MG_KEY_F9;
        case XK_F10: return MG_KEY_F10;
        case XK_F11: return MG_KEY_F11;
        case XK_F12: return MG_KEY_F12;

        case XK_Left:      return MG_KEY_LEFT;
        case XK_Right:     return MG_KEY_RIGHT;
        case XK_Up:        return MG_KEY_UP;
        case XK_Down:      return MG_KEY_DOWN;
        case XK_Home:      return MG_KEY_HOME;
        case XK_End:       return MG_KEY_END;
        case XK_Page_Up:   return MG_KEY_PAGE_UP;
        case XK_Page_Down: return MG_KEY_PAGE_DOWN;
        case XK_Insert:    return MG_KEY_INSERT;
        case XK_Delete:    return MG_KEY_DELETE;

        case XK_Shift_L:   return MG_KEY_LSHIFT;
        case XK_Shift_R:   return MG_KEY_RSHIFT;
        case XK_Control_L: return MG_KEY_LCONTROL;
        case XK_Control_R: return MG_KEY_RCONTROL;
        case XK_Alt_L:     return MG_KEY_LALT;
        case XK_Alt_R:     return MG_KEY_RALT;
        case XK_Super_L:   return MG_KEY_LSUPER;
        case XK_Super_R:   return MG_KEY_RSUPER;
        case XK_Escape:    return MG_KEY_ESCAPE;
        case XK_Tab:       return MG_KEY_TAB;
        case XK_Return:    return MG_KEY_ENTER;
        case XK_BackSpace: return MG_KEY_BACKSPACE;
        case XK_space:     return MG_KEY_SPACE;

        case XK_comma:        return MG_KEY_COMMA;
        case XK_period:       return MG_KEY_PERIOD;
        case XK_minus:        return MG_KEY_MINUS;
        case XK_equal:        return MG_KEY_EQUAL;
        case XK_semicolon:    return MG_KEY_SEMICOLON;
        case XK_grave:        return MG_KEY_GRAVE;
        case XK_slash:        return MG_KEY_SLASH;
        case XK_backslash:    return MG_KEY_BACKSLASH;
        case XK_bracketleft:  return MG_KEY_LBRACKET;
        case XK_bracketright: return MG_KEY_RBRACKET;
        case XK_apostrophe:   return MG_KEY_APOSTROPHE;

        default: return MG_KEY_MAX;
    }
}
 
int32_t mg_app_run(const mg_app_init_info *info)
{
    platform.display = XOpenDisplay(NULL);
    if (!platform.display)
    {
        fprintf(stderr, "Failed to open X display\n");
        return 1;
    }
 
    platform.screen = DefaultScreen(platform.display);
 
    const int32_t width  = info->width  ? (int32_t)info->width  : 800;
    const int32_t height = info->height ? (int32_t)info->height : 600;
 
    platform.window_width  = width;
    platform.window_height = height;
 
    Window root = RootWindow(platform.display, platform.screen);
 
    XSetWindowAttributes swa = {0};
    swa.background_pixel = BlackPixel(platform.display, platform.screen);
    swa.event_mask =
        StructureNotifyMask | ExposureMask |
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask |
        PointerMotionMask;
 
    platform.window = XCreateWindow(
        platform.display,
        root,
        0, 0,
        (unsigned int)width, (unsigned int)height,
        0,
        DefaultDepth(platform.display, platform.screen),
        InputOutput,
        DefaultVisual(platform.display, platform.screen),
        CWBackPixel | CWEventMask,
        &swa
    );
 
    if (!platform.window)
    {
        fprintf(stderr, "Failed to create X window\n");
        XCloseDisplay(platform.display);
        return 1;
    }
 
    if (info->title)
        XStoreName(platform.display, platform.window, info->title);
 
    platform.wm_delete_window = XInternAtom(platform.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(platform.display, platform.window, &platform.wm_delete_window, 1);
 
    XMapWindow(platform.display, platform.window);
    XFlush(platform.display);
 
    platform.on_event_call = info->events.event;
    platform.running = true;
 
    platform.time = mg_xlib_get_time();
    platform.delta_time = 0.0f;
 
    if (info->events.start)
        info->events.start();
 
    while (platform.running)
    {
        while (XPending(platform.display))
        {
            XEvent xev;
            XNextEvent(platform.display, &xev);
 
            switch (xev.type)
            {
                case ConfigureNotify:
                {
                    XConfigureEvent *ce = &xev.xconfigure;
                    if (ce->width  != platform.window_width ||
                        ce->height != platform.window_height)
                    {
                        platform.window_width  = ce->width;
                        platform.window_height = ce->height;
                        mg_app_call_event(&(mg_app_event){
                            .window_width  = platform.window_width,
                            .window_height = platform.window_height,
                            .type          = MG_APP_EVENT_RESIZE
                        });
                    }
                }
                break;

                case KeyPress:
                case KeyRelease:
                {
                    KeySym sym = XLookupKeysym(&xev.xkey, 0);
                    mg_key key = mg_xlib_translate_key(sym);

                    bool pressed = (xev.type == KeyPress);

                    mg_app_input_process_key(key, pressed);

                    mg_app_call_event(&(mg_app_event){
                        .key  = key,
                        .type = pressed ? MG_APP_EVENT_KEY_DOWN : MG_APP_EVENT_KEY_UP
                    });

                    if (pressed)
                    {
                        char buf[4] = {0};
                        int len = XLookupString(&xev.xkey, buf, sizeof(buf), NULL, NULL);
                        if (len == 1 && (unsigned char)buf[0] >= 32)
                            mg_app_call_event(&(mg_app_event){
                                .ch   = buf[0],
                                .type = MG_APP_EVENT_CHAR
                            });
                    }
                }
                break;

                case ButtonPress:
                case ButtonRelease:
                {
                    bool pressed = (xev.type == ButtonPress);

                    if (xev.xbutton.button == Button4 || xev.xbutton.button == Button5)
                    {
                        if (pressed)
                        {
                            int8_t delta = (xev.xbutton.button == Button4) ? 1 : -1;
                            input_state.mouse.delta = delta;
                            mg_app_call_event(&(mg_app_event){
                                .scroll_delta = delta,
                                .type         = MG_APP_EVENT_MOUSE_SCROLL
                            });
                        }
                        break;
                    }

                    mg_mouse_button mb = MG_MOUSE_BUTTON_MAX;
                    switch (xev.xbutton.button)
                    {
                        case Button1: mb = MG_MOUSE_BUTTON_LEFT;   break;
                        case Button2: mb = MG_MOUSE_BUTTON_MIDDLE; break;
                        case Button3: mb = MG_MOUSE_BUTTON_RIGHT;  break;
                    }

                    if (mb != MG_MOUSE_BUTTON_MAX)
                    {
                        mg_app_input_process_mouse_button(mb, pressed);
                        mg_app_call_event(&(mg_app_event){
                            .mouse_button = mb,
                            .type         = pressed ? MG_APP_EVENT_MOUSE_DOWN : MG_APP_EVENT_MOUSE_UP
                        });
                    }
                }
                break;

                case MotionNotify:
                {
                    input_state.mouse.x = xev.xmotion.x;
                    input_state.mouse.y = xev.xmotion.y;
                    mg_app_call_event(&(mg_app_event){
                        .mouse_x = xev.xmotion.x,
                        .mouse_y = xev.xmotion.y,
                        .type    = MG_APP_EVENT_MOUSE_MOVE
                    });
                }
                break;
 
                case ClientMessage:
                    if ((Atom)xev.xclient.data.l[0] == platform.wm_delete_window)
                        platform.running = false;
                    break;
 
                default:
                    break;
            }
        }
 
        float new_time = mg_xlib_get_time();
        platform.delta_time = new_time - platform.time;
        platform.time = new_time;
 
        if (info->events.update)
            info->events.update();

        mg_app_input_reset();
    }
 
    if (info->events.end)
        info->events.end();
 
    XDestroyWindow(platform.display, platform.window);
    XCloseDisplay(platform.display);
    return 0;
}
 
void mg_app_close(void)
{
    platform.running = false;
}
 
float mg_app_time(void)
{
    return platform.time;
}
 
float mg_app_delta_time(void)
{
    return platform.delta_time;
}
 
int32_t mg_app_width(void)
{
    return platform.window_width;
}
 
int32_t mg_app_height(void)
{
    return platform.window_height;
}
 
void *mg_app_handle(void)
{
    return (void*)&platform;
}
 
#endif // Platform

#endif // MGAPP_IMPL

#ifdef __cplusplus
}
#endif