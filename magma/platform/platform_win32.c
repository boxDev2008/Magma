#include "platform/platform.h"

#include "core/input.h"
#include "core/event.h"
#include "core/event_types.h"

#if MG_PLATFORM_WINDOWS

#include <windowsx.h>

#include <stdio.h>

typedef struct mg_win32_handle_info
{
    HINSTANCE h_instance;
    HWND hwnd;

    uint32_t window_width, window_height;
}
mg_win32_handle_info_t;

static double clock_frequency;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param);

WCHAR* mg_create_wide_string_from_utf8(const char* source)
{
    WCHAR* target;
    int count;

    count = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);
    if (!count)
    {
        MessageBox(0, "Failed to convert string from UTF-8", "Error", MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }

    target = (WCHAR*)malloc(count * sizeof(WCHAR));

    if (!MultiByteToWideChar(CP_UTF8, 0, source, -1, target, count))
    {
        MessageBoxA(NULL, "Failed to convert string from UTF-8", "Error!", MB_ICONEXCLAMATION | MB_OK);
        free(target);
        return NULL;
    }

    return target;
}

mg_platform_t *mg_platform_initialize(mg_platform_init_info_t *create_info)
{
    mg_platform_t *platform = (mg_platform_t*)malloc(sizeof(mg_platform_t));
    platform->handle = (mg_win32_handle_info_t*)malloc(sizeof(mg_win32_handle_info_t));
    mg_win32_handle_info_t *state = (mg_win32_handle_info_t*)platform->handle;

    state->window_width = create_info->width;
    state->window_height = create_info->height;

    state->h_instance = GetModuleHandleA(0);

    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = "magma_window_class";

    if (!RegisterClassA(&wc))
    {
        MessageBox(0, "Window refistration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    uint32_t window_x = create_info->position_x;
    uint32_t window_y = create_info->position_y;
    uint32_t window_width = create_info->width;
    uint32_t window_height = create_info->height;

    uint32_t window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    uint32_t window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    window_x += border_rect.left;
    window_y += border_rect.top;

    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

#ifdef _MSC_VER
    WCHAR *wide_title = mg_create_wide_string_from_utf8(create_info->title);

    HWND handle = CreateWindowExA(
        window_ex_style, "magma_window_class", wide_title,
        window_style, window_x, window_y, window_width, window_height,
        0, 0, state->h_instance, platform);

    free(wide_title);
#else
    HWND handle = CreateWindowExA(
        window_ex_style, "magma_window_class", create_info->title,
        window_style, window_x, window_y, window_width, window_height,
        0, 0, state->h_instance, platform);
#endif

    if (handle == 0)
    {
        MessageBoxA(NULL, "Window creation failed", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
    else
        state->hwnd = handle;

    uint32_t should_activate = 1;
    int32_t show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    ShowWindow(state->hwnd, show_window_command_flags);
    
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0 / (double)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);

    return platform;
}

void mg_platform_shutdown(mg_platform_t *platform)
{
    mg_win32_handle_info_t *handle = (mg_win32_handle_info_t*)platform->handle;

    if (handle->hwnd)
    {
        DestroyWindow(handle->hwnd);
        handle->hwnd = 0;
    }

    free(handle);
    free(platform);
}

void mg_platform_poll_messages(mg_platform_t *platform)
{
    mg_win32_handle_info_t *handle = (mg_win32_handle_info_t*)platform->handle;

    MSG message;
    
    while (PeekMessageA(&message, handle->hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
}

LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param)
{
    static mg_platform_t *state = 0;

    switch (msg)
    {
        case WM_ERASEBKGND:
            return 1;
        case WM_NCCREATE:
        {
            CREATESTRUCT *p = (CREATESTRUCT *)l_param;
            state = (mg_platform_t*)p->lpCreateParams;
            DefWindowProc(hwnd, msg, w_param, l_param);
        }
        break;
        case WM_CLOSE:
        {
            mg_application_quit_event_data_t data = {state};
            mg_event_call(MG_EVENT_CODE_APPLICATION_QUIT, (void*)&data);
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
        {
            RECT r;
            GetClientRect(hwnd, &r);

            mg_resized_event_data_t data = {r.right - r.left, r.bottom - r.top};

            mg_win32_handle_info_t *handle = (mg_win32_handle_info_t*)state->handle;
            handle->window_width = data.width; 
            handle->window_height = data.height; 

            mg_event_call(MG_EVENT_CODE_RESIZED, (void*)&data);
        }
        break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            mg_keys_t key = (uint16_t)w_param;

            mg_input_process_key(key, pressed);
        }
        break;
        case WM_CHAR:
            mg_input_process_char((wchar_t)w_param);
        break;
        case WM_MOUSEMOVE:
        {
            int32_t x_position = GET_X_LPARAM(l_param);
            int32_t y_position = GET_Y_LPARAM(l_param);

            mg_input_process_mouse_move(x_position, y_position);
        }
        break;
        case WM_MOUSEWHEEL:
        {
            int32_t z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0)
            {
                z_delta = (z_delta < 0) ? -1 : 1;
                mg_input_process_mouse_wheel(z_delta);
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
            mg_mouse_buttons_t mouse_button = MG_MOUSE_BUTTON_MAX_BUTTONS;
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
        default:
            return DefWindowProc(hwnd, msg, w_param, l_param);
    }
}

double mg_platform_get_time(void)
{
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (double)now_time.QuadPart * clock_frequency;
}

void mg_platform_get_window_size(mg_platform_t *platform, uint32_t *width, uint32_t *height)
{
    mg_win32_handle_info_t *handle = (mg_win32_handle_info_t*)platform->handle;
    *width = handle->window_width;
    *height = handle->window_height;
}

HWND mg_platform_win32_get_handler(mg_platform_t *platform)
{
    mg_win32_handle_info_t *handle = (mg_win32_handle_info_t*)platform->handle;
    return handle->hwnd;
}

#endif