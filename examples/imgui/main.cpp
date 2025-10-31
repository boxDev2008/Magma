#include <magma/core/event.h>
#include <magma/core/event_types.h>

#include <magma/core/app.h>
#include <magma/rendering/renderer.h>

#include <imgui/imgui.h>
#include <imgui/imgui_magma.h>

static uint32_t window_w = 1280;
static uint32_t window_h = 720;

static void on_mouse_moved(mg_mouse_moved_event_data *data)        	{ ImGui::GetIO().AddMousePosEvent(data->x, data->y); }
static void on_mouse_wheel(mg_mouse_wheel_event_data *data)        	{ ImGui::GetIO().AddMouseWheelEvent(0, data->delta); }
static void on_mouse_pressed(mg_mouse_button_event_data *data)     	{ ImGui::GetIO().AddMouseButtonEvent(data->button, true); }
static void on_mouse_released(mg_mouse_button_event_data *data)    	{ ImGui::GetIO().AddMouseButtonEvent(data->button, false); }
static void on_key_pressed(mg_key_event_data *data)                	{ ImGui::GetIO().AddKeyEvent(mg_key_to_imgui_key(data->key), true); }
static void on_key_released(mg_key_event_data *data)               	{ ImGui::GetIO().AddKeyEvent(mg_key_to_imgui_key(data->key), false); }
static void on_char(mg_char_event_data *data)						{ ImGui::GetIO().AddInputCharacter(data->ch); }

void on_resize(mg_resized_event_data *data)
{
    if (data->width == 0 || data->height == 0)
        return;

    mg_swapchain_config_info config_info = {
        .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
        .vsync = true,
        .width = data->width,
        .height = data->height
    };

    mgfx_configure_swapchain(&config_info);

    window_w = data->width;
    window_h = data->height;
}

void on_initialize(int32_t argc, char* const* argv, uint32_t width, uint32_t height)
{
    mg_event_connect(MG_EVENT_CODE_RESIZED, (mg_event)on_resize);
	mg_event_connect(MG_EVENT_CODE_MOUSE_MOVED, (mg_event)on_mouse_moved);
	mg_event_connect(MG_EVENT_CODE_MOUSE_WHEEL, (mg_event)on_mouse_wheel);
	mg_event_connect(MG_EVENT_CODE_MOUSE_BUTTON_PRESSED, (mg_event)on_mouse_pressed);
	mg_event_connect(MG_EVENT_CODE_MOUSE_BUTTON_RELEASED, (mg_event)on_mouse_released);
	mg_event_connect(MG_EVENT_CODE_KEY_PRESSED, (mg_event)on_key_pressed);
	mg_event_connect(MG_EVENT_CODE_KEY_RELEASED, (mg_event)on_key_released);
	mg_event_connect(MG_EVENT_CODE_CHAR, (mg_event)on_char);

	mg_swapchain_config_info swapchain_config = {
        .format = MG_PIXEL_FORMAT_B8G8R8A8_SRGB,
        .vsync = true,
        .width = width,
        .height = height
    };
    
    mgfx_init_info renderer_init_info = {
        .type = MG_RENDERER_TYPE_OPENGL,
        .swapchain_config_info = &swapchain_config
    };
    
    mgfx_initialize(&renderer_init_info);
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;
    io.Fonts->AddFontDefault();
    
    mg_imgui_initialize();
}

void on_shutdown(void)
{
    mg_imgui_shutdown();
    ImGui::DestroyContext();

	mgfx_shutdown();
}

void on_update(float delta_time)
{
    mgfx_begin();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)window_w, (float)window_h);
    io.DeltaTime = delta_time;
    ImGui::NewFrame();
    
    ImGui::ShowDemoWindow();
    
    mg_render_pass_begin_info render_pass_begin_info = {
        .render_area = mg_vec4_new(0.f, 0.f, window_w, window_h),
        .clear_value = mg_vec4_new(1.0f, 0.0f, 0.0f, 1.0f)
    };

    mgfx_begin_default_render_pass(&render_pass_begin_info);
    mgfx_viewport(0, 0, window_w, window_h);
    ImGui::Render();
    mg_imgui_draw(ImGui::GetDrawData());
    mgfx_end_render_pass();
	mgfx_end();
}

static const mg_app_init_info app_info = {
    .name = "ImGui example",
    .width = 1280,
    .height = 720,
    .events = {
        .initialize = on_initialize,
        .shutdown = on_shutdown,
        .update = on_update
    }
};
MG_APP_DEFINE_ENTRY(app_info);