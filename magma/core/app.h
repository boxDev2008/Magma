#pragma once

#include "platform/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_app_init_info
{
    const char *name;
    uint32_t width, height;
    uint32_t flags;

    struct
    {
        void (*initialize)(int32_t argc, char* const* argv, uint32_t width, uint32_t height);
        void (*shutdown)(void);
        void (*update)(float delta_time);
    }
    events;
}
mg_app_init_info;

MG_API void mg_app_run(const mg_app_init_info *info, int32_t argc, char* const* argv);
MG_API void mg_app_close(void);

#define MG_APP_DEFINE_ENTRY(info) \
int32_t main(int32_t argc, char* const* argv)\
{\
    mg_app_run(&info, argc, argv);\
    return 0;\
}

#ifdef __cplusplus
}
#endif