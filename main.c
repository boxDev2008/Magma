#include "magma/core/event.h"
#include "magma/core/event_types.h"

#include "magma/platform/platform.h"

#include "magma/rendering/rhi_renderer.h"
#include "magma/graphics/2d.h"

#include "magma/math/mat4.h"
#include "magma/math/math.h"

static bool is_running = true;

bool is_rendering = true;

void on_application_quit(mg_application_quit_event_data_t *data)
{
    is_running = false;
}

void on_resize(mg_resized_event_data_t *data)
{
    if (data->width == 0 || data->height == 0)
        return;
    
    mg_graphics_2d_resize(data->width, data->height);

    mg_swapchain_config_info_t config_info = {
        .width = data->width,
        .height = data->height,
        .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
        .present_mode = MG_PRESENT_MODE_IMMEDIATE_KHR
    };

    mg_rhi_renderer_configure_swapchain(&config_info);
}

void on_key(mg_key_event_data_t *data)
{

}

void on_mouse_wheel(mg_mouse_wheel_event_data_t *data)
{
    
}

typedef struct UniformBufferObject
{
    mg_mat4_t projection;
}
UniformBufferObject;

typedef struct PushConstantObject
{
    mg_mat4_t model;
}
PushConstantObject;

int main(void)
{
    mg_platform_init_info_t platform_init_info = {
        .title = "Magma",
        .width = 1280,
        .height = 720,
        .position_x = 200,
        .position_y = 100
    };

    mg_event_register(MG_EVENT_CODE_APPLICATION_QUIT, (PFN_on_event)on_application_quit);
    mg_event_register(MG_EVENT_CODE_KEY_PRESSED, on_key);
    mg_event_register(MG_EVENT_CODE_MOUSE_WHEEL, on_mouse_wheel);

    mg_platform_t *platform = mg_platform_initialize(&platform_init_info);
    mg_event_register(MG_EVENT_CODE_RESIZED, (PFN_on_event)on_resize);
    
    mg_renderer_init_info_t renderer_init_info = {
        .platform = platform,
        .type = MG_RENDERER_TYPE_VULKAN,
        .swapchain_config_info = &(mg_swapchain_config_info_t) {
            .width = platform_init_info.width,
            .height = platform_init_info.height,
            .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
            .present_mode = MG_PRESENT_MODE_IMMEDIATE_KHR
        }
    };

    mg_rhi_renderer_initialize(&renderer_init_info);

    mg_graphics_2d_initialize(platform_init_info.width, platform_init_info.height);

    mg_texture_t *texture = mg_graphics_2d_create_texture_from_file("test.png");
    mg_sprite_t *sprite = mg_graphics_2d_create_sprite(0.0f, 0.0f, texture->width / texture->width, texture->height / texture->height, texture);

    float position = 0.0f;

    double current_time, last_time = 0;

    while (is_running)
    {
        current_time = mg_platform_get_time();
        const double delta_time = current_time - last_time;
        last_time = current_time;

        mg_rhi_renderer_begin_frame();

        mg_graphics_2d_begin();

        mg_world_info_t world_info = {
            .camera_zoom = mg_vec2(1.0f, 1.0f),
            .camera_position = mg_vec2(0.0f, 0.0f),
            //.flags = MG_WORLD_FLAG_VIGNETTE
        };

        mg_graphics_2d_begin_world(&world_info);

        position += delta_time * 160.0f;
        if (position > 10000)
            position = 0;

        for (int i = 0; i < 1; i++)
        {
            mg_graphics_2d_draw_sprite(mg_vec2(position + i * 100, i * 100), mg_vec2(160.0f, 160.0f), mg_vec4(1.0f, 1.0f, 1.0f, 1.0f), sprite);
        }

        mg_graphics_2d_end_world();

        mg_graphics_2d_end();

        mg_rhi_renderer_end_frame();
        mg_rhi_renderer_present_frame();
        
        mg_platform_poll_messages(platform);
    }

    mg_graphics_2d_destroy_texture(texture);
    mg_graphics_2d_destroy_sprite(sprite);

    mg_graphics_2d_shutdown();
    mg_rhi_renderer_shutdown();
    mg_platform_shutdown(platform);

    return 0;
}