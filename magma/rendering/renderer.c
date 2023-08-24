#include "renderer.h"

typedef struct mg_renderer_plugin
{
    mg_platform_t *platform;

    void (*initialize)  (mg_platform_t *platform);
    void (*shutdown)    (void);
}
mg_renderer_plugin_t;

mg_renderer_plugin_t plugin;

void mg_renderer_initialize(mg_platform_t *platform, mg_renderer_type_t type)
{
    switch (type)
    {
        case MG_RENDERER_TYPE_OPENGL:
            //plugin.initialize   =   mg_opengl_renderer_initialize;
            //plugin.shutdown     =   mg_opengl_renderer_shutdown;
        break;
    }

    //plugin.initialize(platform);
}

void mg_renderer_shutdown(void)
{
    //plugin.shutdown();
}