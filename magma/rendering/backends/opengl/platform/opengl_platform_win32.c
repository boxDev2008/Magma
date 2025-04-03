#include "opengl_platform.h"

#if MG_PLATFORM_WINDOWS

#include <glad/glad.h>
#include <dwmapi.h>
#include <assert.h>

#pragma comment(lib, "dwmapi.lib")

typedef struct mg_opengl_win32_platform_context
{
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
}
mg_opengl_win32_platform_context;

static mg_opengl_win32_platform_context ctx;

typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

void mg_opengl_platform_initialize(mg_platform *platform)
{
    mg_win32_platform *handle = (mg_win32_platform*)platform;
    ctx.hwnd = handle->hwnd;
    ctx.hdc = GetDC(ctx.hwnd);

    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    INT format = ChoosePixelFormat(ctx.hdc, &pfd);
    SetPixelFormat(ctx.hdc, format, &pfd);

    ctx.hrc = wglCreateContext(ctx.hdc);
    wglMakeCurrent(ctx.hdc, ctx.hrc);

    assert(gladLoadGL());

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
}

void mg_opengl_platform_shutdown(void)
{
    wglMakeCurrent(0, 0);
    wglDeleteContext(ctx.hrc);
    ReleaseDC(ctx.hwnd, ctx.hdc);
}

void mg_opengl_platform_swapbuffers(void)
{
    SwapBuffers(ctx.hdc);
}

void mg_opengl_platform_set_vsync(bool enabled)
{
    wglSwapIntervalEXT(enabled);
}

#endif