#include "platform/platform.h"

#if MG_PLATFORM_LINUX

#include "core/input.h"
#include "core/event.h"
#include "core/event_types.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static double clock_frequency = 0.000000001;
static struct timespec start_time;

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
                KeySym key = XLookupKeysym(&event.xkey, 0);
                bool pressed = (event.type == KeyPress);
                mg_keys mg_key = (mg_keys)key;
                mg_input_process_key(mg_key, pressed);
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