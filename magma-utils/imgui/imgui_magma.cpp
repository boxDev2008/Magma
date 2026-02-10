#include "imgui_magma.h"

#include <magma/core/input.h>
#include <magma/rendering/renderer.h>

#include "imgui_shader.h"

const int32_t MaxVertices = (1<<16);
const int32_t MaxIndices = MaxVertices * 3;

struct ImGuiData
{
    mg_pipeline pipeline;

    mg_image img;
    mg_sampler smp;

    mg_buffer vb;
    mg_buffer ib;

    uint32_t vb_offset;
    uint32_t ib_offset;

    ImDrawVert vertices[MaxVertices];
    ImDrawIdx indices[MaxIndices];
}
*imgui_data;

ImGuiKey mg_key_to_imgui_key(int key)
{
    switch (key)
    {
        case MG_KEY_TAB: return ImGuiKey_Tab;
        case MG_KEY_LEFT: return ImGuiKey_LeftArrow;
        case MG_KEY_RIGHT: return ImGuiKey_RightArrow;
        case MG_KEY_UP: return ImGuiKey_UpArrow;
        case MG_KEY_DOWN: return ImGuiKey_DownArrow;
        case MG_KEY_PAGE_UP: return ImGuiKey_PageUp;
        case MG_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
        case MG_KEY_HOME: return ImGuiKey_Home;
        case MG_KEY_END: return ImGuiKey_End;
        case MG_KEY_INSERT: return ImGuiKey_Insert;
        case MG_KEY_DELETE: return ImGuiKey_Delete;
        case MG_KEY_BACKSPACE: return ImGuiKey_Backspace;
        case MG_KEY_SPACE: return ImGuiKey_Space;
        case MG_KEY_ENTER: return ImGuiKey_Enter;
        case MG_KEY_ESCAPE: return ImGuiKey_Escape;
        case MG_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
        case MG_KEY_COMMA: return ImGuiKey_Comma;
        case MG_KEY_MINUS: return ImGuiKey_Minus;
        case MG_KEY_PERIOD: return ImGuiKey_Period;
        case MG_KEY_SLASH: return ImGuiKey_Slash;
        case MG_KEY_SEMICOLON: return ImGuiKey_Semicolon;
        case MG_KEY_EQUAL: return ImGuiKey_Equal;
        case MG_KEY_LBRACKET: return ImGuiKey_LeftBracket;
        case MG_KEY_BACKSLASH: return ImGuiKey_Backslash;
        case MG_KEY_RBRACKET: return ImGuiKey_RightBracket;
        //case MG_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
        //case MG_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
        //case MG_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
        case MG_KEY_NUMLOCK: return ImGuiKey_NumLock;
        case MG_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
        case MG_KEY_PAUSE: return ImGuiKey_Pause;
        case MG_KEY_NUMPAD0: return ImGuiKey_Keypad0;
        case MG_KEY_NUMPAD1: return ImGuiKey_Keypad1;
        case MG_KEY_NUMPAD2: return ImGuiKey_Keypad2;
        case MG_KEY_NUMPAD3: return ImGuiKey_Keypad3;
        case MG_KEY_NUMPAD4: return ImGuiKey_Keypad4;
        case MG_KEY_NUMPAD5: return ImGuiKey_Keypad5;
        case MG_KEY_NUMPAD6: return ImGuiKey_Keypad6;
        case MG_KEY_NUMPAD7: return ImGuiKey_Keypad7;
        case MG_KEY_NUMPAD8: return ImGuiKey_Keypad8;
        case MG_KEY_NUMPAD9: return ImGuiKey_Keypad9;
        case MG_KEY_DECIMAL: return ImGuiKey_KeypadDecimal;
        case MG_KEY_DIVIDE: return ImGuiKey_KeypadDivide;
        case MG_KEY_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case MG_KEY_SUBTRACT: return ImGuiKey_KeypadSubtract;
        case MG_KEY_ADD: return ImGuiKey_KeypadAdd;
        //case MG_KEY_ENTER: return ImGuiKey_KeypadEnter;
        case MG_KEY_NUMPAD_EQUAL: return ImGuiKey_KeypadEqual;
        case MG_KEY_LSHIFT: return ImGuiKey_LeftShift;
        case MG_KEY_LCONTROL: return ImGuiKey_LeftCtrl;
        case MG_KEY_LALT: return ImGuiKey_LeftAlt;
        case MG_KEY_LSUPER: return ImGuiKey_LeftSuper;
        case MG_KEY_RSHIFT: return ImGuiKey_RightShift;
        case MG_KEY_RCONTROL: return ImGuiKey_RightCtrl;
        case MG_KEY_RALT: return ImGuiKey_RightAlt;
        case MG_KEY_RSUPER: return ImGuiKey_RightSuper;
        //case MG_KEY_MENU: return ImGuiKey_Menu;
        case MG_KEY_0: return ImGuiKey_0;
        case MG_KEY_1: return ImGuiKey_1;
        case MG_KEY_2: return ImGuiKey_2;
        case MG_KEY_3: return ImGuiKey_3;
        case MG_KEY_4: return ImGuiKey_4;
        case MG_KEY_5: return ImGuiKey_5;
        case MG_KEY_6: return ImGuiKey_6;
        case MG_KEY_7: return ImGuiKey_7;
        case MG_KEY_8: return ImGuiKey_8;
        case MG_KEY_9: return ImGuiKey_9;
        case MG_KEY_A: return ImGuiKey_A;
        case MG_KEY_B: return ImGuiKey_B;
        case MG_KEY_C: return ImGuiKey_C;
        case MG_KEY_D: return ImGuiKey_D;
        case MG_KEY_E: return ImGuiKey_E;
        case MG_KEY_F: return ImGuiKey_F;
        case MG_KEY_G: return ImGuiKey_G;
        case MG_KEY_H: return ImGuiKey_H;
        case MG_KEY_I: return ImGuiKey_I;
        case MG_KEY_J: return ImGuiKey_J;
        case MG_KEY_K: return ImGuiKey_K;
        case MG_KEY_L: return ImGuiKey_L;
        case MG_KEY_M: return ImGuiKey_M;
        case MG_KEY_N: return ImGuiKey_N;
        case MG_KEY_O: return ImGuiKey_O;
        case MG_KEY_P: return ImGuiKey_P;
        case MG_KEY_Q: return ImGuiKey_Q;
        case MG_KEY_R: return ImGuiKey_R;
        case MG_KEY_S: return ImGuiKey_S;
        case MG_KEY_T: return ImGuiKey_T;
        case MG_KEY_U: return ImGuiKey_U;
        case MG_KEY_V: return ImGuiKey_V;
        case MG_KEY_W: return ImGuiKey_W;
        case MG_KEY_X: return ImGuiKey_X;
        case MG_KEY_Y: return ImGuiKey_Y;
        case MG_KEY_Z: return ImGuiKey_Z;
        case MG_KEY_F1: return ImGuiKey_F1;
        case MG_KEY_F2: return ImGuiKey_F2;
        case MG_KEY_F3: return ImGuiKey_F3;
        case MG_KEY_F4: return ImGuiKey_F4;
        case MG_KEY_F5: return ImGuiKey_F5;
        case MG_KEY_F6: return ImGuiKey_F6;
        case MG_KEY_F7: return ImGuiKey_F7;
        case MG_KEY_F8: return ImGuiKey_F8;
        case MG_KEY_F9: return ImGuiKey_F9;
        case MG_KEY_F10: return ImGuiKey_F10;
        case MG_KEY_F11: return ImGuiKey_F11;
        case MG_KEY_F12: return ImGuiKey_F12;
        case MG_KEY_F13: return ImGuiKey_F13;
        case MG_KEY_F14: return ImGuiKey_F14;
        case MG_KEY_F15: return ImGuiKey_F15;
        case MG_KEY_F16: return ImGuiKey_F16;
        case MG_KEY_F17: return ImGuiKey_F17;
        case MG_KEY_F18: return ImGuiKey_F18;
        case MG_KEY_F19: return ImGuiKey_F19;
        case MG_KEY_F20: return ImGuiKey_F20;
        case MG_KEY_F21: return ImGuiKey_F21;
        case MG_KEY_F22: return ImGuiKey_F22;
        case MG_KEY_F23: return ImGuiKey_F23;
        case MG_KEY_F24: return ImGuiKey_F24;
        default: return ImGuiKey_None;
    }
}

void mg_imgui_initialize(void)
{
    imgui_data = new ImGuiData;

    mg_buffer_create_info vb_create_info = {
        .size = sizeof(imgui_data->vertices),
        .usage = MG_BUFFER_USAGE_VERTEX,
        .access = MG_ACCESS_TYPE_GPU
    };
    imgui_data->vb = mgfx_create_buffer(&vb_create_info);

    mg_buffer_create_info ib_create_info = {
        .size = sizeof(imgui_data->indices),
        .usage = MG_BUFFER_USAGE_INDEX,
        .access = MG_ACCESS_TYPE_GPU
    };
    imgui_data->ib = mgfx_create_buffer(&ib_create_info);

	ImGuiIO& io = ImGui::GetIO();

    unsigned char* font_pixels;
    int32_t font_width, font_height;
    io.Fonts->GetTexDataAsRGBA32(&font_pixels, &font_width, &font_height);

    mg_image_create_info img_create_info = { };
    img_create_info.format = MG_PIXEL_FORMAT_RGBA8_UNORM;
    img_create_info.type = MG_IMAGE_TYPE_2D;
    img_create_info.width = font_width;
    img_create_info.height = font_height;
    img_create_info.usage = MG_IMAGE_USAGE_COLOR_ATTACHMENT;
    imgui_data->img = mgfx_create_image(&img_create_info);

    mg_image_update_info update_info = { };
    update_info.width = font_width;
    update_info.height = font_height;
    update_info.data = font_pixels;
    update_info.bpp = 4;
    mgfx_update_image(imgui_data->img, &update_info);

    mg_sampler_create_info smp_create_info = { };
    smp_create_info.min_filter = MG_SAMPLER_FILTER_NEAREST;
    smp_create_info.mag_filter = MG_SAMPLER_FILTER_NEAREST;
    smp_create_info.address_mode_u = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    smp_create_info.address_mode_v = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    smp_create_info.address_mode_w = MG_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    imgui_data->smp = mgfx_create_sampler(&smp_create_info);

	io.Fonts->SetTexID((ImTextureID)imgui_data->img);

    mg_pipeline_create_info pipeline_create_info = {
        .shader = get_imgui_shader(mgfx_get_type()),
        .vertex_layout = {
            .attributes = {
                { .location = 0, .offset = offsetof(ImDrawVert, pos), .format = MG_VERTEX_FORMAT_FLOAT2, },
                { .location = 1, .offset = offsetof(ImDrawVert, uv), .format = MG_VERTEX_FORMAT_FLOAT2, },
                { .location = 2,  .offset = offsetof(ImDrawVert, col), .format = MG_VERTEX_FORMAT_UBYTE4N }
            },
            .stride = sizeof(ImDrawVert)
        },
    
        .primitive_topology = MG_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .polygon_mode = MG_POLYGON_MODE_FILL,
        .cull_mode = MG_CULL_MODE_NONE,
        .front_face = MG_FRONT_FACE_CW,
        
        .color_blend = {
            .src_color_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
            .dst_color_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .color_blend_op = MG_BLEND_OP_ADD,
            .src_alpha_blend_factor = MG_BLEND_FACTOR_SRC_ALPHA,
            .dst_alpha_blend_factor = MG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .alpha_blend_op = MG_BLEND_OP_ADD,
            .blend_enabled = true
        }
    };
    
    imgui_data->pipeline =
        mgfx_create_pipeline(&pipeline_create_info);
}

void mg_imgui_shutdown(void)
{
    mgfx_destroy_pipeline(imgui_data->pipeline);
    mgfx_destroy_sampler(imgui_data->smp);
    mgfx_destroy_image(imgui_data->img);
    mgfx_destroy_buffer(imgui_data->vb);
    mgfx_destroy_buffer(imgui_data->ib);
    delete imgui_data;
}

void mg_imgui_draw(ImDrawData* draw_data)
{
    assert(draw_data);
    if (draw_data->CmdListsCount == 0)
        return;

    imgui_data->vb_offset = 0;
    imgui_data->ib_offset = 0;

	ImGuiIO &io = ImGui::GetIO();

    mgfx_bind_pipeline(imgui_data->pipeline);

    struct 
    {
        ImVec2 disp_size;
    }
    uniform_data;
    uniform_data.disp_size.x = io.DisplaySize.x;
    uniform_data.disp_size.y = io.DisplaySize.y;

    mgfx_bind_uniforms(0, sizeof(uniform_data), &uniform_data);
    mgfx_bind_vertex_buffer(imgui_data->vb);
    mgfx_bind_index_buffer(imgui_data->ib, MG_INDEX_TYPE_UINT16);

    for (int cl_index = 0; cl_index < draw_data->CmdListsCount; cl_index++)
    {
        const ImDrawList* cl = draw_data->CmdLists[cl_index];

        int32_t i;
        for (i = 0; i < cl->VtxBuffer.size(); i++)
        {
            imgui_data->vertices[imgui_data->vb_offset] = cl->VtxBuffer[i];
            imgui_data->vb_offset++;
        }

        for (i = 0; i < cl->IdxBuffer.size(); i++)
        {
            imgui_data->indices[imgui_data->ib_offset] = cl->IdxBuffer[i];
            imgui_data->ib_offset++;
        }
    }

    mgfx_update_buffer(imgui_data->vb, sizeof(imgui_data->vertices), imgui_data->vertices);
    mgfx_update_buffer(imgui_data->ib, sizeof(imgui_data->indices), imgui_data->indices);

    int global_vtx_offset = 0;
    int global_idx_offset = 0;

	ImTextureID global_tex_id = io.Fonts->TexID.GetTexID();
	mgfx_bind_image((mg_image)global_tex_id, imgui_data->smp, 0);
    
    for (int cl_index = 0; cl_index < draw_data->CmdListsCount; cl_index++)
    {
        const ImDrawList* cl = draw_data->CmdLists[cl_index];

        int vtx_offset = global_vtx_offset;
        for (const ImDrawCmd& pcmd : cl->CmdBuffer)
        {
            if (pcmd.UserCallback)
                pcmd.UserCallback(cl, &pcmd);
            else
            {
                int scissor_x = (int) (pcmd.ClipRect.x);
                int scissor_y = (int) (pcmd.ClipRect.y);
                int scissor_w = (int) (pcmd.ClipRect.z - pcmd.ClipRect.x);
                int scissor_h = (int) (pcmd.ClipRect.w - pcmd.ClipRect.y);

				const ImTextureID &tex_id = pcmd.GetTexID();
				if (pcmd.GetTexID() != global_tex_id)
				{
					global_tex_id = tex_id;
	                mgfx_bind_image((mg_image)global_tex_id, imgui_data->smp, 0);
				}

                mgfx_scissor(scissor_x, scissor_y, scissor_w, scissor_h);
                mgfx_draw_indexed(pcmd.ElemCount, global_idx_offset, global_vtx_offset);
            }
            global_idx_offset += pcmd.ElemCount;
        }

        global_vtx_offset += cl->VtxBuffer.Size;
    }
}