#include "magma/core/event.h"
#include "magma/core/event_types.h"

#include "magma/platform/platform.h"

#include "magma/rendering/rhi_renderer.h"
#include "magma/graphics/2d.h"

#include "magma/math/mat4.h"
#include "magma/math/math.h"

#include "magma/physics/2d/world_2d.h"

#include <math.h>

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
        .present_mode = MG_PRESENT_MODE_FIFO_KHR
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
            .present_mode = MG_PRESENT_MODE_FIFO_KHR
        }
    };

    mg_rhi_renderer_initialize(&renderer_init_info);

    mg_graphics_2d_initialize(platform_init_info.width, platform_init_info.height);

    mg_texture_t *texture = mg_graphics_2d_create_texture_from_file("a.png");
    mg_sprite_t *sprite = mg_graphics_2d_create_sprite(0.0f, 0.0f, texture->width / texture->width, texture->height / texture->height, texture);

    float position = 0.0f;

    double current_time, last_time = 0;

    mg_physics_world_2d_initialize(mg_vec2(0.0f, 0.1f), 16);

    mg_physics_body_2d_t *body1[8];

    for (int i = 0; i < 8; i++)
    {
        body1[i] = mg_physics_world_2d_create_rect_body(mg_vec4(-8.0f, -8.0f, 8.0f, 8.0f));
        body1[i]->position = mg_vec2(0.0f, -100.0f);
        body1[i]->fall_speed = mg_vec2(0.0f, 1.0f);
        body1[i]->mass = 1.0f;
        body1[i]->is_static = false;
    }

    mg_physics_body_2d_t *body2 = mg_physics_world_2d_create_rect_body(mg_vec4(-80.0f, -80.0f, 80.0f, 80.0f));
    body2->position = mg_vec2(0.0f, 300.0f);
    body2->mass = 1.0f;
    body2->is_static = true;

    while (is_running)
    {
        current_time = mg_platform_get_time();
        const double delta_time = current_time - last_time;
        last_time = current_time;

        mg_physics_world_2d_step(delta_time);

        mg_rhi_renderer_begin_frame();

        mg_graphics_2d_begin();

        mg_world_info_t world_info = {
            .camera_zoom = mg_vec2(1.0f, 1.0f),
            .camera_position = mg_vec2(0.0f, 0.0f),
            //.flags = MG_WORLD_FLAG_VIGNETTE
        };

        mg_graphics_2d_begin_world(&world_info);

        mg_graphics_2d_end_world();

        int32_t x, y;
        mg_input_get_mouse_position(&x, &y);

        body2->position.x = x - 640;
        body2->position.y = y - 360;

        for (int i = 0; i < 8; i++)
            mg_graphics_2d_draw_rect(mg_vec2(body1[i]->position.x, body1[i]->position.y), mg_vec2(16.0f, 16.0f), mg_vec4(1.0f, 0.8f, 1.0f, 1.0f));

        mg_graphics_2d_draw_rect(mg_vec2(body2->position.x, body2->position.y), mg_vec2(160.0f, 160.0f), mg_vec4(1.0f, 0.8f, 1.0f, 1.0f));

        mg_graphics_2d_end();

        mg_rhi_renderer_end_frame();
        mg_rhi_renderer_present_frame();
        
        mg_platform_poll_messages(platform);
    }

    mg_physics_world_2d_shutdown();

    mg_graphics_2d_destroy_texture(texture);
    mg_graphics_2d_destroy_sprite(sprite);

    mg_graphics_2d_shutdown();
    mg_rhi_renderer_shutdown();
    mg_platform_shutdown(platform);

    return 0;
}