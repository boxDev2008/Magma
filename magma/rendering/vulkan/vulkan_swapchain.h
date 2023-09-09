#include "vulkan_renderer.h"
#include "vulkan_types.inl"

#include "math/math.h"

#include <stdlib.h>
#include <assert.h>

VkSurfaceFormatKHR  mg_vulkan_choose_swap_surface_format    (const VkSurfaceFormatKHR *available_formats, uint32_t available_format_count);
VkExtent2D          mg_vulkan_choose_swap_extent            (const VkSurfaceCapabilitiesKHR *capabilities, int32_t width, int32_t height);

void                mg_vulkan_create_swapchain              (mg_swapchain_config_info_t *config_info);
void                mg_vulkan_cleanup_swapchain             (void);

void                mg_vulkan_configure_swapchain           (mg_swapchain_config_info_t *config_info);
VkFramebuffer       mg_vulkan_get_swapchain_framebuffer     (void);