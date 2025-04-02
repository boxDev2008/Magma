#include "opengl_platform.h"

#if MG_PLATFORM_LINUX

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <glad/gl.h>

typedef struct mg_opengl_egl_platform_context
{
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
}
mg_opengl_egl_platform_context;

static mg_opengl_egl_platform_context ctx;

void mg_opengl_platform_initialize(mg_platform *platform)
{
    mg_x11_platform *handle = (mg_x11_platform*)platform;
    ctx.display = eglGetDisplay((EGLNativeDisplayType)handle->display);
    eglInitialize(ctx.display, NULL, NULL);

    EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(ctx.display, configAttribs, &config, 1, &numConfigs);

    ctx.surface = eglCreateWindowSurface(ctx.display, config, (EGLNativeWindowType)handle->window, NULL);
    
    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3, // Request OpenGL 3.0+
        EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_API);
    ctx.context = eglCreateContext(ctx.display, config, EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(ctx.display, ctx.surface, ctx.surface, ctx.context);

    gladLoadGL((GLADloadfunc)eglGetProcAddress);
}

void mg_opengl_platform_shutdown(void)
{
    eglMakeCurrent(ctx.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(ctx.display, ctx.context);
    eglDestroySurface(ctx.display, ctx.surface);
    eglTerminate(ctx.display);
}

void mg_opengl_platform_swapbuffers(void)
{
    eglSwapBuffers(ctx.display, ctx.surface);
}

void mg_opengl_platform_set_vsync(bool enabled)
{
    eglSwapInterval(ctx.display, enabled ? 1 : 0);
}

#endif