#include "ui.h"
#include "fonts/consola.h"

#include "core/input.h"

#define MG_GRAPHICS_UI_TEXT_COLOR (mg_vec4_t) { 1.0f, 1.0f, 1.0f, 1.0f }
#define MG_GRAPHICS_UI_WINDOW_BG_COLOR (mg_vec4_t) { 0.01f, 0.01f, 0.01f, 0.9f }
#define MG_GRAPHICS_UI_WINDOW_TITLEBAR_BG_COLOR (mg_vec4_t) { 0.02f, 0.02f, 0.02f, 0.9f }
#define MG_GRAPHICS_UI_BUTTON_COLOR (mg_vec4_t) { 0.005f, 0.005f, 0.005f, 0.9f }

typedef struct mg_ui_data
{
	mg_font_t *font;

	char *title;
	uint32_t width, height;

	float current_x;
	float current_y;
}
mg_ui_data_t;

static mg_ui_data_t ui_data;

void mg_graphics_ui_initialize(void)
{
	ui_data.font = mg_graphics_create_font(consola_font);
}

void mg_graphics_ui_shutdown(void)
{
	mg_graphics_destroy_font(ui_data.font);
}

void mg_graphics_ui_begin_window(const char *title, mg_vec2_t size)
{
	ui_data.title = title;
	ui_data.width = size.x;
	ui_data.height = size.y;

	mg_mat4_t model = mg_mat4_identity();
	model = mg_mat4_translate(model, (mg_vec3_t) { 0.5f, 0.5f, 0.0f });
	model = mg_mat4_scale(model, (mg_vec3_t) { ui_data.width, ui_data.height, 1.0f });
	mg_graphics_2d_draw_rect(model, MG_GRAPHICS_UI_WINDOW_BG_COLOR);

	model = mg_mat4_identity();
	model = mg_mat4_translate(model, (mg_vec3_t) { 0.5f, 0.5f, 0.0f });
	model = mg_mat4_scale(model, (mg_vec3_t) { ui_data.width, 22.0f, 1.0f });
	model = mg_mat4_translate(model, (mg_vec3_t) { 0.0f, -22.0f, 0.0f });
	mg_graphics_2d_draw_rect(model, MG_GRAPHICS_UI_WINDOW_TITLEBAR_BG_COLOR);

	model = mg_mat4_identity();
	model = mg_mat4_scale(model, (mg_vec3_t) { 20.0f, 20.0f, 1.0f });
	model = mg_mat4_translate(model, (mg_vec3_t) { 4.0f, -4.0f, 0.0f });
	mg_graphics_2d_draw_text(model, MG_GRAPHICS_UI_TEXT_COLOR, ui_data.title, ui_data.font);

	mg_graphics_2d_draw_line(mg_vec2(0.0f, -22.0f), mg_vec2(ui_data.width, -22.0f), mg_vec4(0.1f, 0.1f, 0.1f, 1.0f));
	mg_graphics_2d_draw_line(mg_vec2(0.0f, -22.0f), mg_vec2(0.0f, ui_data.height), mg_vec4(0.1f, 0.1f, 0.1f, 1.0f));
	mg_graphics_2d_draw_line(mg_vec2(0.0f, ui_data.height), mg_vec2(ui_data.width, ui_data.height), mg_vec4(0.1f, 0.1f, 0.1f, 1.0f));
	mg_graphics_2d_draw_line(mg_vec2(ui_data.width, -22.0f), mg_vec2(ui_data.width, ui_data.height), mg_vec4(0.1f, 0.1f, 0.1f, 1.0f));
	mg_graphics_2d_draw_line(mg_vec2(0.0f, 0.0f), mg_vec2(ui_data.width, 0.0f), mg_vec4(0.1f, 0.1f, 0.1f, 1.0f));

	ui_data.current_x = 16.0f;
	ui_data.current_y = 16.0f;

	mg_graphics_ui_button("Button", mg_vec2(144, 24));
}

void mg_graphics_ui_end_window(void)
{

}

void mg_graphics_ui_text(const char* text)
{

}

bool mg_graphics_ui_button(const char *text, mg_vec2_t size)
{
	mg_mat4_t model = mg_mat4_identity();
	model = mg_mat4_translate(model, (mg_vec3_t) { 0.5f, 0.5f, 0.0f });
	model = mg_mat4_scale(model, (mg_vec3_t) { size.x, size.y, 1.0f });
	model = mg_mat4_translate(model, (mg_vec3_t) { ui_data.current_x, ui_data.current_y, 0.0f });
	mg_graphics_2d_draw_rect(model, MG_GRAPHICS_UI_BUTTON_COLOR);

	model = mg_mat4_identity();
	model = mg_mat4_scale(model, (mg_vec3_t) { 20.0f, 20.0f, 1.0f });
	model = mg_mat4_translate(model, (mg_vec3_t) { ui_data.current_x + 8.0f, ui_data.current_y + 19.0f, 0.0f });
	mg_graphics_2d_draw_text(model, MG_GRAPHICS_UI_TEXT_COLOR, text, ui_data.font);

	mg_graphics_2d_draw_line(mg_vec2(ui_data.current_x, ui_data.current_y), mg_vec2(ui_data.current_x, ui_data.current_y + size.y), mg_vec4(0.1f, 0.1f, 0.1f, 1.0f));
	mg_graphics_2d_draw_line(mg_vec2(ui_data.current_x + size.x, ui_data.current_y), mg_vec2(ui_data.current_x + size.x, ui_data.current_y + size.y), mg_vec4(0.1f, 0.1f, 0.1f, 1.0f));
	mg_graphics_2d_draw_line(mg_vec2(ui_data.current_x, ui_data.current_y), mg_vec2(ui_data.current_x + size.x, ui_data.current_y), mg_vec4(0.1f, 0.1f, 0.1f, 1.0f));
	mg_graphics_2d_draw_line(mg_vec2(ui_data.current_x, ui_data.current_y + size.y), mg_vec2(ui_data.current_x + size.x, ui_data.current_y + size.y), mg_vec4(0.1f, 0.1f, 0.1f, 1.0f));
	return false;
}

void mg_graphics_ui_checkbox(const char *text, bool *value)
{

}