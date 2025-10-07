#include "vulkan_platform.h"

#if MG_PLATFORM_WINDOWS

#include "../vulkan_renderer.h"
#include <assert.h>

void mg_vulkan_create_surface(void)
{
    mg_win32_platform *handle = (mg_win32_platform*)mg_platform_get_handle();

    VkWin32SurfaceCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    create_info.hwnd = handle->hwnd;
    create_info.hinstance = handle->h_instance;

    VkResult result = vkCreateWin32SurfaceKHR(vk_ctx.instance, &create_info, NULL, &vk_ctx.surface);
    assert(result == VK_SUCCESS);
}

#endif