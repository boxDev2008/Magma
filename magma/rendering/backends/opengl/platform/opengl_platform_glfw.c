#include "opengl_platform.h"

#include <glad/glad.h>

#if MG_PLATFORM_LINUX

#include <GLFW/glfw3.h>

typedef struct mg_opengl_glfw_platform_context
{
    GLFWwindow *gwin;
}
mg_opengl_glfw_platform_context;

static mg_opengl_glfw_platform_context ctx;

void mg_opengl_platform_initialize(mg_platform *platform)
{
    mg_glfw_platform *handle = (mg_glfw_platform*)platform;

    glfwMakeContextCurrent(handle->gwin);

    if (!gladLoadGL())
        printf("gladLoadGL failed\n");
    
    ctx.gwin = handle->gwin;
}

void mg_opengl_platform_shutdown()
{
}

void mg_opengl_platform_swapbuffers(void)
{
    glfwSwapBuffers(ctx.gwin);
}

void mg_opengl_platform_set_vsync(bool enabled)
{
    glfwSwapInterval(enabled);
}

#endif