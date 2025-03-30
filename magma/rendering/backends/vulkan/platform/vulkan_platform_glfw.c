#include "vulkan_platform.h"

#if MG_PLATFORM_LINUX

#include "../vulkan_renderer.h"
#include <assert.h>

#include <GLFW/glfw3.h>

void mg_vulkan_create_surface(mg_platform *platform)
{
    mg_glfw_platform *handle = (mg_glfw_platform*)platform;

    VkResult result = glfwCreateWindowSurface(vk_ctx.instance, handle->gwin, NULL, &vk_ctx.surface);
    assert(result == VK_SUCCESS);
}

#endif