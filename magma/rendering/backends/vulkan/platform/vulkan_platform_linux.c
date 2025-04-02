#include "vulkan_platform.h"

#if MG_PLATFORM_LINUX

#include "../vulkan_renderer.h"
#include <assert.h>
#include <X11/Xlib.h>

void mg_vulkan_create_surface(mg_platform *platform)
{
    mg_x11_platform *handle = (mg_x11_platform*)platform;

    VkXlibSurfaceCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR};
    create_info.dpy = handle->display;
    create_info.window = handle->window;

    VkResult result = vkCreateXlibSurfaceKHR(vk_ctx.instance, &create_info, NULL, &vk_ctx.surface);
    assert(result == VK_SUCCESS);
}

#endif