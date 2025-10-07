#include "opengl_platform.h"

#if MG_PLATFORM_EMSCRIPTEN

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>

static EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = 0;

void mg_opengl_platform_initialize(void)
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.alpha = EM_TRUE;
    attrs.depth = EM_TRUE;
    attrs.stencil = EM_TRUE;
    attrs.antialias = EM_TRUE;
    attrs.enableExtensionsByDefault = EM_TRUE;

    context = emscripten_webgl_create_context("#canvas", &attrs);
    if (context <= 0)
    {
        emscripten_log(EM_LOG_ERROR, "Failed to create WebGL2 context");
        return;
    }

    emscripten_webgl_make_context_current(context);
}

void mg_opengl_platform_shutdown(void)
{
    if (!context)
        return;
    emscripten_webgl_destroy_context(context);
    context = 0;
}

void mg_opengl_platform_swapbuffers(void)
{
    
}

void mg_opengl_platform_set_vsync(bool enabled)
{
    
}

#endif