#include "platform/platform.h"

#include "core/input.h"
#include "core/event.h"
#include "core/event_types.h"

#include <stdio.h>
#include <assert.h>

#include <GLFW/glfw3.h>

static void callback_quit(GLFWwindow *gwin)
{
    mg_quit_event_data data = {glfwGetWindowUserPointer(gwin)};
    mg_event_call(MG_EVENT_CODE_APPLICATION_QUIT, (void*)&data);
}

static void callback_resize(GLFWwindow *gwin, int width, int height)
{
    mg_resized_event_data data = {width, height};
    mg_event_call(MG_EVENT_CODE_RESIZED, (void*)&data);
}

static void callback_key(GLFWwindow *gwin, int key, int scancode, int action, int mods)
{
    bool pressed = action == GLFW_PRESS;
    mg_keys keys = (mg_keys)key;
    mg_input_process_key(keys, pressed);
}

static void callback_char(GLFWwindow *gwin, unsigned int codepoint)
{
    mg_input_process_char((char)codepoint);
}

static void callback_mouse_pos(GLFWwindow *gwin, double xpos, double ypos)
{
    mg_input_process_mouse_move((int32_t)xpos, (int32_t)ypos);
}

static void callback_mouse_button(GLFWwindow *gwin, int button, int action, int mods)
{
    bool pressed = action == GLFW_PRESS;
    mg_input_process_mouse_button((mg_mouse_buttons)button, pressed);
}

static void callback_mouse_wheel(GLFWwindow* window, double xoffset, double yoffset)
{
    mg_input_process_mouse_wheel((int8_t)yoffset);
}

mg_platform *mg_platform_initialize(mg_platform_init_info *init_info)
{
    mg_glfw_platform *platform = (mg_glfw_platform*)malloc(sizeof(mg_glfw_platform));

    platform->window_width = init_info->width;
    platform->window_height = init_info->height;

    if (!glfwInit())
        return NULL;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    platform->gwin = glfwCreateWindow(init_info->width, init_info->height, init_info->title, NULL, NULL);
    assert(platform->gwin != NULL);

    glfwSetWindowUserPointer(platform->gwin, platform);

    glfwSetWindowCloseCallback(platform->gwin, callback_quit);
    glfwSetFramebufferSizeCallback(platform->gwin, callback_resize);
    glfwSetKeyCallback(platform->gwin, callback_key);
    glfwSetCharCallback(platform->gwin, callback_char);
    glfwSetCursorPosCallback(platform->gwin, callback_mouse_pos);
    glfwSetMouseButtonCallback(platform->gwin, callback_mouse_button);
    glfwSetScrollCallback(platform->gwin, callback_mouse_wheel);

    return platform;
}

void mg_platform_shutdown(mg_platform *platform)
{
    mg_glfw_platform *handle = (mg_glfw_platform*)platform;

    glfwDestroyWindow(handle->gwin);
    glfwTerminate();

    free(platform);
}

void mg_platform_poll_events(mg_platform *platform)
{
    glfwPollEvents();
}

double mg_get_time(void)
{
    return glfwGetTime();
}
