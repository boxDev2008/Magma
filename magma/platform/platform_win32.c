#include "platform/platform.h"

#if MG_PLATFORM_WINDOWS

#include "core/input.h"
#include "core/event.h"
#include "core/event_types.h"

#include <windowsx.h>

#include <stdio.h>
#include <assert.h>

static mg_win32_platform *platform;

static float clock_frequency;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param);

static void mg_create_wide_string_from_utf8(const char *source, WCHAR *target)
{
    uint32_t count;

    count = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);
    if (!count)
    {
        MessageBoxA(0, (LPCSTR)"Failed to convert string from UTF-8", "Error", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    if (!MultiByteToWideChar(CP_UTF8, 0, source, -1, target, count))
    {
        MessageBoxA(NULL, (LPCSTR)"Failed to convert string from UTF-8", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
}

void mg_platform_initialize(mg_platform_init_info *init_info)
{
    platform = (mg_win32_platform*)malloc(sizeof(mg_win32_platform));
    platform->h_instance = GetModuleHandleA(0);

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

    HICON icon = LoadIcon(platform->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = platform->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = "magma_window_class";

    if (!RegisterClassA(&wc))
    {
        MessageBoxA(0, (LPCSTR)"Window registration failed", (LPCSTR)"Error", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    uint32_t window_x = CW_USEDEFAULT;
    uint32_t window_y = CW_USEDEFAULT;
    uint32_t window_width = init_info->width;
    uint32_t window_height = init_info->height;

    uint32_t window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
    uint32_t window_ex_style = WS_EX_APPWINDOW;

    if (init_info->flags & MG_PLATFORM_FLAG_RESIZABLE)
        window_style |= WS_THICKFRAME | WS_MAXIMIZEBOX;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

#ifdef _MSC_VER
    WCHAR wide_title[128];
    mg_create_wide_string_from_utf8(init_info->title, wide_title);

    HWND handle = CreateWindowExA(
        window_ex_style, (LPCSTR)"magma_window_class", (LPCSTR)wide_title,
        window_style, window_x, window_y, window_width, window_height,
        0, 0, platform->h_instance, platform);
#else
    HWND handle = CreateWindowExA(
        window_ex_style, (LPCSTR)"magma_window_class", (LPCSTR)init_info->title,
        window_style, window_x, window_y, window_width, window_height,
        0, 0, platform->h_instance, platform);
#endif

    if (handle == 0)
    {
        MessageBoxA(NULL, "Window creation failed", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
    else
        platform->hwnd = handle;

    if (!(init_info->flags & MG_PLATFORM_FLAG_HIDE_WINDOW))
    {
        uint32_t should_activate = 1;
        int32_t show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
        ShowWindow(platform->hwnd, show_window_command_flags);
    }

    if (init_info->flags & MG_PLATFORM_FLAG_HIDE_CURSOR)
        ShowCursor(FALSE);

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0f / (float)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);

    platform->window_width = init_info->width;
    platform->window_height = init_info->height;
}

void mg_platform_shutdown(void)
{
    assert(platform->hwnd != NULL);

    DestroyWindow(platform->hwnd);
    platform->hwnd = 0;

    UnregisterClassA("magma_window_class", platform->h_instance);

    free(platform);
}

void mg_platform_poll_events(void)
{
    mg_win32_platform *handle = (mg_win32_platform*)platform;

    MSG message;
    
    while (PeekMessageA(&message, platform->hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
        case WM_ERASEBKGND:
            return 1;
        break;
        case WM_CLOSE:
        {
            mg_quit_event_data data = {(mg_platform*)platform};
            mg_event_call(MG_EVENT_CODE_QUIT, (void*)&data);
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
        {
            RECT r;
            GetClientRect(hwnd, &r);

            mg_resized_event_data data = {r.right - r.left, r.bottom - r.top};

            platform->window_width = data.width;
            platform->window_height = data.height;

            mg_event_call(MG_EVENT_CODE_RESIZED, (void*)&data);
        }
        break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            mg_keys key = (uint16_t)w_param;

            mg_input_process_key(key, pressed);
        }
        break;
        case WM_CHAR:
            mg_input_process_char((char)w_param);
        break;
        case WM_MOUSEMOVE:
        {
            int32_t x = GET_X_LPARAM(l_param);
            int32_t y = GET_Y_LPARAM(l_param);

            mg_input_process_mouse_move(x, y);
        }
        break;
        case WM_MOUSEWHEEL:
        {
            int32_t delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (delta != 0)
            {
                delta = (delta < 0) ? -1 : 1;
                mg_input_process_mouse_wheel(delta);
            }
        }
        break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            bool pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            mg_mouse_buttons mouse_button = MG_MOUSE_BUTTON_MAX;
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

            mg_input_process_mouse_button(mouse_button, pressed);
        }
        break;
    }
    return DefWindowProc(hwnd, msg, w_param, l_param);
}

float mg_get_time(void)
{
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (float)(now_time.QuadPart - start_time.QuadPart) * clock_frequency;
}

void mg_platform_show_window(void)
{
    ShowWindow(platform->hwnd, SW_SHOW);
}

void mg_platform_get_primary_screen_size(uint32_t *width, uint32_t *height)
{
    *width = GetSystemMetrics(SM_CXSCREEN);
    *height = GetSystemMetrics(SM_CYSCREEN);
}

void mg_platform_get_virtual_screen_size(uint32_t *width, uint32_t *height)
{
    *width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    *height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

mg_dynamic_library *mg_platform_load_library(const char *library_name)
{
    return (mg_dynamic_library*)LoadLibraryA(library_name);
}

mg_proc_address *mg_platform_get_proc_address(mg_dynamic_library *library, const char *proc_name)
{
    return (mg_proc_address*)GetProcAddress((HMODULE)library, proc_name);
}

void mg_platform_unload_library(mg_dynamic_library *library)
{
    if (library)
        FreeLibrary((HMODULE)library);
}

mg_platform *mg_platform_get_handle(void)
{
    return (mg_platform*)platform;
}

#endif