#include "vulkan_renderer.h"
#include "vulkan_types.h"

// VkSurfaceFormatKHR  mg_vulkan_choose_swap_surface_format    (const VkSurfaceFormatKHR *available_formats, uint32_t available_format_count);
VkExtent2D  mg_vulkan_choose_swap_extent    (const VkSurfaceCapabilitiesKHR *capabilities, int32_t width, int32_t height);

void        mg_vulkan_create_swapchain      (uint32_t width, uint32_t height, bool vsync);
void        mg_vulkan_cleanup_swapchain     (void);

void        mg_vulkan_reset                 (uint32_t width, uint32_t height, bool vsync);