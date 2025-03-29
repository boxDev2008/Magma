#pragma once

#include "platform/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_app_init_info
{
    const char *name;
    uint32_t width, height;

    struct
    {
        void (*initialize)(int32_t argc, char* const* argv, uint32_t width, uint32_t height, mg_platform *platform);
        void (*shutdown)(void);
        void (*update)(float delta_time);
    }
    events;
}
mg_app_init_info;

MG_API void mg_app_run(mg_app_init_info *info, int32_t argc, char* const* argv);

#define MG_APP_DEFINE_ENTRY(info, ...)\
int32_t main(int32_t argc, char* const* argv)\
{\
    mg_app_init_info init_info = info, __VA_ARGS__;\
    mg_app_run(&init_info, argc, argv);\
    return 0;\
}

#ifdef __cplusplus
}
#endif