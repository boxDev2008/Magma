#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_platform mg_platform;
typedef struct mg_dynamic_library mg_dynamic_library;
typedef struct mg_proc_address mg_proc_address;

typedef struct mg_platform_init_info
{
    const char *title;
    uint32_t width, height;
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
#endif

MG_API mg_platform         *mg_platform_initialize              (mg_platform_init_info *init_info);
MG_API void                 mg_platform_shutdown                (mg_platform *platform);

MG_API void                 mg_platform_poll_events		        (mg_platform *platform);
MG_API double               mg_get_time                         (void);

MG_API mg_dynamic_library  *mg_platform_load_library            (const char *library_name);
MG_API mg_proc_address     *mg_platform_get_proc_address        (mg_dynamic_library *library, const char *proc_name);
MG_API void                 mg_platform_unload_library          (mg_dynamic_library *library);

#ifdef __cplusplus
}
#endif