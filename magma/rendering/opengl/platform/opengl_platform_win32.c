#include "opengl_platform.h"

#if MG_PLATFORM_WINDOWS

#include <GL/gl.h>
#include <windows.h>
#include <assert.h>

typedef struct mg_win32_handle_info
{
    HINSTANCE h_instance;
    HWND hwnd;
}
mg_win32_handle_info_t;

static HWND hwnd;
static HDC hdc;
static HGLRC hrc;

void mg_opengl_platform_initialize(mg_platform_t *platform)
{
    mg_win32_handle_info_t *handle = (mg_win32_handle_info_t*)platform->handle;
    hwnd = handle->hwnd;
    hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    hrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hrc);

    gladLoadGL();
}

void mg_opengl_platform_shutdown(void)
{
    ReleaseDC(hwnd, hdc);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
}

void mg_opengl_platform_swapbuffers(void)
{
    SwapBuffers(hdc);
}

#endif