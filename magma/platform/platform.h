#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_platform mg_platform;
typedef struct mg_dynamic_library mg_dynamic_library;
typedef struct mg_proc_address mg_proc_address;

typedef enum mg_platform_flags
{
    MG_PLATFORM_FLAG_NONE = 0,
    //MG_PLATFORM_FLAG_FULLSCREEN = 1 << 0,
    MG_PLATFORM_FLAG_RESIZABLE = 1 << 1,
    MG_PLATFORM_FLAG_HIDE_CURSOR = 1 << 2,
    MG_PLATFORM_FLAG_HIDE_WINDOW = 1 << 3
}
mg_platform_flags;

typedef struct mg_platform_init_info
{
    const char *title;
    uint32_t width, height;
    uint32_t flags;
}
mg_platform_init_info;

#if MG_PLATFORM_WINDOWS
#include <windows.h>
typedef struct mg_win32_platform
{
    HINSTANCE h_instance;
    HWND hwnd;
    uint32_t window_width, window_height;
}
mg_win32_platform;
#elif MG_PLATFORM_LINUX
#include <X11/Xlib.h>
typedef struct mg_x11_platform
{
    Display *display;
    Window window;
    int32_t screen;
    Window root_window;
    int32_t window_width;
    int32_t window_height;
	Atom wm_delete_window;
	Cursor hidden_cursor;
}
mg_x11_platform;
#endif

MG_API void                 mg_platform_initialize              (mg_platform_init_info *init_info);
MG_API void                 mg_platform_shutdown                (void);

MG_API void                 mg_platform_poll_events		        (void);
MG_API float                mg_get_time                         (void);

MG_API void                 mg_platform_show_window             (void);

MG_API void                 mg_platform_get_primary_screen_size (uint32_t *width, uint32_t *height);
MG_API void                 mg_platform_get_virtual_screen_size (uint32_t *width, uint32_t *height);

MG_API mg_dynamic_library  *mg_platform_load_library            (const char *library_name);
MG_API mg_proc_address     *mg_platform_get_proc_address        (mg_dynamic_library *library, const char *proc_name);
MG_API void                 mg_platform_unload_library          (mg_dynamic_library *library);

MG_API inline mg_platform  *mg_platform_get_handle              (void);

#ifdef __cplusplus
}
#endif