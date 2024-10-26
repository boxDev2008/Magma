#include "vulkan_platform.h"

#if MG_PLATFORM_WINDOWS

#include "../vulkan_renderer.h"
#include "platform/platform.h"

#include <windows.h>
#include <assert.h>

typedef struct mg_win32_platform
{
    HINSTANCE h_instance;
    HWND hwnd;
}
mg_win32_platform;

void mg_vulkan_create_surface(mg_platform *platform)
{
    mg_win32_platform *handle = (mg_win32_platform*)platform;

    VkWin32SurfaceCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    create_info.hwnd = handle->hwnd;
    create_info.hinstance = handle->h_instance;

    VkResult result = vkCreateWin32SurfaceKHR(vk_context.instance, &create_info, NULL, &vk_context.surface);
    assert(result == VK_SUCCESS);
}

#endif