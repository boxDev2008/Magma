#pragma once

#include "imgui.h"

void mg_imgui_initialize(void);
void mg_imgui_shutdown(void);
void mg_imgui_draw(ImDrawData* draw_data);
ImGuiKey mg_key_to_imgui_key(int key);