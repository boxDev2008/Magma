#include <magma/core/event.h>
#include <magma/core/event_types.h>

#include <magma/core/app.h>
#include <magma/rendering/renderer.h>

#include <imgui/imgui.h>
#include <imgui/imgui_magma.h>

static uint32_t window_w = 1280;
static uint32_t window_h = 720;

static void on_mouse_moved(mg_mouse_moved_event_data *data)        	{ ImGui::GetIO().AddMousePosEvent((float)data->x, (float)data->y); }
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

    window_w = data->width;
    window_h = data->height;

    mgfx_reset(window_w, window_h, true);
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
    
    mgfx_init_info renderer_init_info = {
        .type = MG_RENDERER_TYPE_OPENGL,
        .width = width,
        .height = height,
        .vsync = true
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
    
    mg_render_pass_bind_info render_pass_bind_info = {
        .render_area = mg_vec4i_new(0, 0, window_w, window_h),
        .clear_value = mg_vec4_new(0.0f, 0.0f, 0.0f, 1.0f)
    };

    mgfx_bind_render_pass(NULL, &render_pass_bind_info);
    mgfx_viewport(0, 0, window_w, window_h);
    ImGui::Render();
    mg_imgui_draw(ImGui::GetDrawData());
	mgfx_end();
}

static const mg_app_init_info app_info = {
    .name = "ImGui Example",
    .flags = MG_PLATFORM_FLAG_RESIZABLE,
    .width = window_w,
    .height = window_h,
    .events = {
        .initialize = on_initialize,
        .shutdown = on_shutdown,
        .update = on_update
    }
};
MG_APP_DEFINE_ENTRY(app_info);