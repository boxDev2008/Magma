#pragma once

#include "base.h"

#include "math/vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

#if MG_PLATFORM_WINDOWS
#include <windows.h>
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
MG_API HWND mg_platform_win32_get_handler(mg_platform *platform);
#endif

MG_API mg_platform         *mg_platform_initialize          (mg_platform_init_info *init_info);
MG_API void                 mg_platform_shutdown            (mg_platform *platform);

MG_API void                 mg_platform_poll_events		    (mg_platform *platform);
MG_API double               mg_platform_get_time            (void);

MG_API void                 mg_platform_get_window_size     (mg_platform *platform, uint32_t *width, uint32_t *height);

MG_API mg_dynamic_library  *mg_platform_load_library        (const char *library_name);
MG_API mg_proc_address     *mg_platform_get_proc_address    (mg_dynamic_library *library, const char *proc_name);
MG_API void                 mg_platform_unload_library      (mg_dynamic_library *library);

#ifdef __cplusplus
}
#endif