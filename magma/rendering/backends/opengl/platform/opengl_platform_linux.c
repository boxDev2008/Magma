#include "opengl_platform.h"

#if MG_PLATFORM_LINUX

#include <glad/glad.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <assert.h>

typedef struct mg_opengl_x11_platform_context
{
    Display *display;
    Window window;
    GLXContext glx_context;
    GLXFBConfig fb_config;
    GLXDrawable drawable;
}
mg_opengl_x11_platform_context;

static mg_opengl_x11_platform_context ctx;

static PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = NULL;

void mg_opengl_platform_initialize(mg_platform *platform)
{
    mg_x11_platform *handle = (mg_x11_platform*)platform;
    ctx.display = handle->display;
    ctx.window = handle->window;

    static int visual_attribs[] = { GLX_X_RENDERABLE, True,
                                    GLX_DRAWABLE_TYPE, GLX_WINDOW,
                                    GLX_RENDER_TYPE, GLX_RGBA_BIT,
                                    GLX_DOUBLEBUFFER, True,
                                    None };
    int fb_count;
    GLXFBConfig *fb_config = glXChooseFBConfig(ctx.display, DefaultScreen(ctx.display), visual_attribs, &fb_count);
    assert(fb_count > 0 && "No suitable framebuffer config found");

    ctx.glx_context = glXCreateNewContext(ctx.display, fb_config[0], GLX_RGBA_TYPE, NULL, True);
    assert(ctx.glx_context != NULL && "Failed to create OpenGL context");

    ctx.drawable = ctx.window;
    glXMakeCurrent(ctx.display, ctx.drawable, ctx.glx_context);

    gladLoadGL();

	glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
}

void mg_opengl_platform_shutdown(void)
{
    glXMakeCurrent(ctx.display, None, NULL);
    glXDestroyContext(ctx.display, ctx.glx_context);
}

void mg_opengl_platform_swapbuffers(void)
{
    glXSwapBuffers(ctx.display, ctx.drawable);
}

void mg_opengl_platform_set_vsync(bool enabled)
{
	glXSwapIntervalEXT(ctx.display, ctx.drawable, enabled ? 1 : 0);
}

#endif