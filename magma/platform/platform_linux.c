#include "platform/platform.h"

#if MG_PLATFORM_LINUX

#include "core/input.h"
#include "core/event.h"
#include "core/event_types.h"

#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <dlfcn.h>

static double clock_frequency = 0.000000001;
static struct timespec start_time;

static mg_keys mg_translate_keycode(uint32_t keycode)
{
    switch (keycode)
	{
    case XK_BackSpace:
        return MG_KEY_BACKSPACE;
    case XK_Return:
        return MG_KEY_ENTER;
    case XK_Tab:
        return MG_KEY_TAB;

    case XK_Pause:
        return MG_KEY_PAUSE;
    case XK_Caps_Lock:
        return MG_KEY_CAPITAL;

    case XK_Escape:
        return MG_KEY_ESCAPE;

    case XK_Mode_switch:
        return MG_KEY_MODECHANGE;

    case XK_space:
        return MG_KEY_SPACE;
    case XK_Prior:
        return MG_KEY_PAGEUP;
    case XK_Next:
        return MG_KEY_PAGEDOWN;
    case XK_End:
        return MG_KEY_END;
    case XK_Home:
        return MG_KEY_HOME;
    case XK_Left:
        return MG_KEY_LEFT;
    case XK_Up:
        return MG_KEY_UP;
    case XK_Right:
        return MG_KEY_RIGHT;
    case XK_Down:
        return MG_KEY_DOWN;
    case XK_Select:
        return MG_KEY_SELECT;
    case XK_Print:
        return MG_KEY_PRINT;
    case XK_Execute:
        return MG_KEY_EXECUTE;

	case XK_Insert:
        return MG_KEY_INSERT;
    case XK_Delete:
        return MG_KEY_DELETE;
    case XK_Help:
        return MG_KEY_HELP;

    case XK_Meta_L:
    case XK_Super_L:
        return MG_KEY_LSUPER;
    case XK_Meta_R:
    case XK_Super_R:
        return MG_KEY_RSUPER;

    case XK_KP_0:
        return MG_KEY_NUMPAD0;
    case XK_KP_1:
        return MG_KEY_NUMPAD1;
    case XK_KP_2:
        return MG_KEY_NUMPAD2;
    case XK_KP_3:
        return MG_KEY_NUMPAD3;
    case XK_KP_4:
        return MG_KEY_NUMPAD4;
    case XK_KP_5:
        return MG_KEY_NUMPAD5;
    case XK_KP_6:
        return MG_KEY_NUMPAD6;
    case XK_KP_7:
        return MG_KEY_NUMPAD7;
    case XK_KP_8:
        return MG_KEY_NUMPAD8;
    case XK_KP_9:
        return MG_KEY_NUMPAD9;
    case XK_multiply:
        return MG_KEY_MULTIPLY;
    case XK_KP_Add:
        return MG_KEY_ADD;
    case XK_KP_Separator:
        return MG_KEY_SEPARATOR;
    case XK_KP_Subtract:
        return MG_KEY_SUBTRACT;
    case XK_KP_Decimal:
        return MG_KEY_DECIMAL;
    case XK_KP_Divide:
        return MG_KEY_DIVIDE;
    case XK_F1:
        return MG_KEY_F1;
    case XK_F2:
        return MG_KEY_F2;
    case XK_F3:
        return MG_KEY_F3;
    case XK_F4:
        return MG_KEY_F4;
    case XK_F5:
        return MG_KEY_F5;
    case XK_F6:
        return MG_KEY_F6;
    case XK_F7:
        return MG_KEY_F7;
    case XK_F8:
        return MG_KEY_F8;
    case XK_F9:
        return MG_KEY_F9;
    case XK_F10:
        return MG_KEY_F10;
    case XK_F11:
        return MG_KEY_F11;
    case XK_F12:
        return MG_KEY_F12;
    case XK_F13:
        return MG_KEY_F13;
    case XK_F14:
        return MG_KEY_F14;
    case XK_F15:
        return MG_KEY_F15;
    case XK_F16:
        return MG_KEY_F16;
    case XK_F17:
        return MG_KEY_F17;
    case XK_F18:
        return MG_KEY_F18;
    case XK_F19:
        return MG_KEY_F19;
    case XK_F20:
        return MG_KEY_F20;
    case XK_F21:
        return MG_KEY_F21;
    case XK_F22:
        return MG_KEY_F22;
    case XK_F23:
        return MG_KEY_F23;
    case XK_F24:
        return MG_KEY_F24;

    case XK_Num_Lock:
        return MG_KEY_NUMLOCK;
    case XK_Scroll_Lock:
        return MG_KEY_SCROLL;

    case XK_KP_Equal:
        return MG_KEY_NUMPAD_EQUAL;

    case XK_Shift_L:
        return MG_KEY_LSHIFT;
    case XK_Shift_R:
        return MG_KEY_RSHIFT;
    case XK_Control_L:
        return MG_KEY_LCONTROL;
    case XK_Control_R:
        return MG_KEY_RCONTROL;
    case XK_Alt_L:
        return MG_KEY_LALT;
    case XK_Alt_R:
        return MG_KEY_RALT;

    case XK_semicolon:
        return MG_KEY_SEMICOLON;
    case XK_plus:
        return MG_KEY_EQUAL;
    case XK_comma:
        return MG_KEY_COMMA;
    case XK_minus:
        return MG_KEY_MINUS;
    case XK_period:
        return MG_KEY_PERIOD;
    case XK_slash:
        return MG_KEY_SLASH;
    case XK_grave:
        return MG_KEY_GRAVE;

    case XK_0:
        return MG_KEY_0;
    case XK_1:
        return MG_KEY_1;
    case XK_2:
        return MG_KEY_2;
    case XK_3:
        return MG_KEY_3;
    case XK_4:
        return MG_KEY_4;
    case XK_5:
        return MG_KEY_5;
    case XK_6:
        return MG_KEY_6;
    case XK_7:
        return MG_KEY_7;
    case XK_8:
        return MG_KEY_8;
    case XK_9:
        return MG_KEY_9;

    case XK_a:
    case XK_A:
        return MG_KEY_A;
    case XK_b:
    case XK_B:
        return MG_KEY_B;
    case XK_c:
    case XK_C:
        return MG_KEY_C;
    case XK_d:
    case XK_D:
        return MG_KEY_D;
    case XK_e:
    case XK_E:
        return MG_KEY_E;
    case XK_f:
    case XK_F:
        return MG_KEY_F;
    case XK_g:
    case XK_G:
        return MG_KEY_G;
    case XK_h:
    case XK_H:
        return MG_KEY_H;
    case XK_i:
    case XK_I:
        return MG_KEY_I;
    case XK_j:
    case XK_J:
        return MG_KEY_J;
    case XK_k:
    case XK_K:
        return MG_KEY_K;
    case XK_l:
    case XK_L:
        return MG_KEY_L;
    case XK_m:
    case XK_M:
        return MG_KEY_M;
    case XK_n:
    case XK_N:
        return MG_KEY_N;
    case XK_o:
    case XK_O:
        return MG_KEY_O;
    case XK_p:
    case XK_P:
        return MG_KEY_P;
    case XK_q:
    case XK_Q:
        return MG_KEY_Q;
    case XK_r:
    case XK_R:
        return MG_KEY_R;
    case XK_s:
    case XK_S:
        return MG_KEY_S;
    case XK_t:
    case XK_T:
        return MG_KEY_T;
    case XK_u:
    case XK_U:
        return MG_KEY_U;
    case XK_v:
    case XK_V:
        return MG_KEY_V;
    case XK_w:
    case XK_W:
        return MG_KEY_W;
    case XK_x:
    case XK_X:
        return MG_KEY_X;
    case XK_y:
    case XK_Y:
        return MG_KEY_Y;
    case XK_z:
    case XK_Z:
        return MG_KEY_Z;

    default:
        return 0;
    }
}

mg_platform *mg_platform_initialize(mg_platform_init_info *init_info)
{
    mg_x11_platform *platform = (mg_x11_platform*)malloc(sizeof(mg_x11_platform));

    platform->display = XOpenDisplay(NULL);
    if (!platform->display)
    {
        fprintf(stderr, "Failed to open X11 display\n");
		free(platform);
        return NULL;
    }

    platform->screen = DefaultScreen(platform->display);
    platform->root_window = RootWindow(platform->display, platform->screen);

    XSetWindowAttributes attributes;
    attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

    platform->window = XCreateWindow(
        platform->display, platform->root_window,
        0, 0, init_info->width, init_info->height, 0,
        CopyFromParent, InputOutput, CopyFromParent,
        CWEventMask, &attributes
    );

    if (!platform->window)
    {
        fprintf(stderr, "Failed to create X11 window\n");
        XCloseDisplay(platform->display);
		free(platform);
        return NULL;
    }

    XStoreName(platform->display, platform->window, init_info->title);
    XMapWindow(platform->display, platform->window);

	platform->wm_delete_window = XInternAtom(platform->display, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(platform->display, platform->window, &platform->wm_delete_window, 1);
	
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    platform->window_width = init_info->width;
    platform->window_height = init_info->height;

    return platform;
}

void mg_platform_shutdown(mg_platform *platform)
{
    mg_x11_platform *handle = (mg_x11_platform*)platform;

    assert(handle->window != 0);	

    XDestroyWindow(handle->display, handle->window);
    XCloseDisplay(handle->display);

    free(platform);
}

void mg_platform_poll_events(mg_platform *platform)
{
    mg_x11_platform *handle = (mg_x11_platform*)platform;

    XEvent event;
    while (XPending(handle->display) > 0)
    {
        XNextEvent(handle->display, &event);
        
        switch (event.type)
        {
            case Expose:
                break;
            case ConfigureNotify:
            {
                XConfigureEvent *resize_event = (XConfigureEvent*)&event;
                mg_resized_event_data data = {resize_event->width, resize_event->height};
                handle->window_width = data.width;
                handle->window_height = data.height;
                mg_event_call(MG_EVENT_CODE_RESIZED, (void*)&data);
            }
            break;
			case KeyPress:
            case KeyRelease:
            {
                int32_t ret;
                KeySym *key = XGetKeyboardMapping(handle->display, (KeyCode)event.xkey.keycode, 1, &ret);
                bool pressed = (event.type == KeyPress);
                mg_keys mg_key = mg_translate_keycode(*key);
                mg_input_process_key(mg_key, pressed);
                XFree(key);
            }
            break;
            case KeymapNotify:
            {
                XRefreshKeyboardMapping(&event.xmapping);
            }
            break;
            case ButtonPress:
            case ButtonRelease:
            {
                bool pressed = (event.type == ButtonPress);
                mg_mouse_buttons mouse_button = MG_MOUSE_BUTTON_MAX_BUTTONS;

                switch (event.xbutton.button)
                {
                    case Button1:
                        mouse_button = MG_MOUSE_BUTTON_LEFT;
                        break;
                    case Button2:
                        mouse_button = MG_MOUSE_BUTTON_MIDDLE;
                        break;
                    case Button3:
                        mouse_button = MG_MOUSE_BUTTON_RIGHT;
                        break;
                }

                mg_input_process_mouse_button(mouse_button, pressed);
            }
            break;
            case MotionNotify:
            {
                int32_t x = event.xmotion.x;
                int32_t y = event.xmotion.y;
                mg_input_process_mouse_move(x, y);
            }
            break;
			case ClientMessage:
			{
				if (event.xclient.message_type == handle->wm_delete_window)
				{
					mg_quit_event_data data = {platform};
					mg_event_call(MG_EVENT_CODE_QUIT, (void*)&data);
				}
			}
			break;
            default:
                break;
        }
    }
}

double mg_get_time(void)
{
    struct timespec now_time;
    clock_gettime(CLOCK_MONOTONIC, &now_time);
    return (double)(now_time.tv_sec - start_time.tv_sec) + (now_time.tv_nsec - start_time.tv_nsec) * clock_frequency;
}

mg_dynamic_library *mg_platform_load_library(const char *library_name)
{
    return dlopen(library_name, RTLD_LAZY);
}

mg_proc_address *mg_platform_get_proc_address(mg_dynamic_library *library, const char *proc_name)
{
    return (mg_proc_address*)dlsym(library, proc_name);
}

void mg_platform_unload_library(mg_dynamic_library *library)
{
    if (library)
        dlclose(library);
}

#endif