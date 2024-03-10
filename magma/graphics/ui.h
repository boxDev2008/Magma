#pragma once

#include "graphics.h"
#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

MG_API void mg_graphics_ui_initialize		(void);
MG_API void mg_graphics_ui_shutdown			(void);

MG_API void mg_graphics_ui_begin_window		(const char* title, mg_vec2_t size);
MG_API void mg_graphics_ui_end_window		(void);

MG_API void mg_graphics_ui_text				(const char *text);
MG_API bool mg_graphics_ui_button			(const char *text, mg_vec2_t size);
MG_API void mg_graphics_ui_checkbox			(const char *text, bool *value);

#ifdef __cplusplus
}
#endif