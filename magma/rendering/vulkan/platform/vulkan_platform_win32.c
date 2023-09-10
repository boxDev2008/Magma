#include "vulkan_platform.h"

#if MG_PLATFORM_WINDOWS

#include "../vulkan_renderer.h"

#include <windows.h>
#include <assert.h>

typedef struct mg_win32_handle_info
{
    HINSTANCE h_instance;
    HWND hwnd;
}
mg_win32_handle_info_t;

void mg_vulkan_create_surface(mg_platform_t *platform)
{
    mg_win32_handle_info_t *handle = (mg_win32_handle_info_t*)platform->handle;

    VkWin32SurfaceCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    createInfo.hwnd = handle->hwnd;
    createInfo.hinstance = handle->h_instance;

    VkResult result = vkCreateWin32SurfaceKHR(vulkan_context.instance, &createInfo, NULL, &vulkan_context.surface);
    assert(result == VK_SUCCESS);
}

#endif