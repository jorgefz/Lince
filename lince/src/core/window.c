
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/core.h"
#include "core/window.h"

#include "event/event.h"
#include "event/key_event.h"
#include "event/mouse_event.h"
#include "event/window_event.h"

#include "renderer/context.h"



static void GLFWErrorCallback(int error, const char* description) {
    LINCE_INFO("GLFW ERROR %d -> ", error);
    LINCE_ASSERT(0, description);
}

// forward declare to call from CreateWindow
static void SetGLFWCallbacks();

// Public functions

LinceWindow* LinceWindow_Create(unsigned int width, unsigned int height){

    LINCE_ASSERT(glfwInit(), "Failed to initialise GLFW");
    GLFWwindow* handle = glfwCreateWindow(width, height, "Lince Window", NULL, NULL);
    if (!handle) {
        glfwTerminate();
        LINCE_ASSERT(0, "Failed to create window");
    }
    GLContextInit(handle); // load GLAD

    glfwSwapInterval(1); // activate VSYNC
    glViewport(0, 0, width, height);

    int glfw_major, glfw_minor, glfw_rev;
    glfwGetVersion(&glfw_major, &glfw_minor, &glfw_rev);
    LINCE_INFO("GLFW Version %d.%d.%d\n", glfw_major, glfw_minor, glfw_rev);

    LinceWindow* window = malloc(sizeof(LinceWindow));
    LINCE_ASSERT(window, "Failed to allocate memory");

    *window = (LinceWindow){
        .handle = handle,
        .height = height,
        .width = width,
        .initialised = 1,
        .title = "Lince Window",
        .event_callback = NULL
    };

    glfwSetWindowUserPointer((GLFWwindow*)window->handle, window);
    glfwSetErrorCallback(GLFWErrorCallback);

    SetGLFWCallbacks(window);

    return window;
}

unsigned int LinceWindow_ShouldClose(LinceWindow* window){
    return glfwWindowShouldClose((GLFWwindow*)(window->handle));
}

void LinceWindow_Update(LinceWindow* window){
    //glfwSwapBuffers((GLFWwindow*)(window->handle));
    GLContextSwapBuffers(window->handle);
	glfwPollEvents();
}

void LinceWindow_Destroy(LinceWindow* window){
    glfwSetErrorCallback(NULL);
    if (window->initialised) glfwTerminate();
    if (window->handle) glfwDestroyWindow((GLFWwindow*)(window->handle));
	free(window);
}

void LinceWindow_SetEventCallback(LinceWindow* window, LinceEventCallbackFn func){
    window->event_callback = func;
}



/*
    ----- Event Callbacks -----
*/

static void WindowResizeCallback(GLFWwindow* wptr, int width, int height){
    glViewport(0, 0, width, height);
    
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    w->width = (unsigned int)width;
    w->height = (unsigned int)height;

    LinceEvent e = LinceNewWindowResizeEvent(width, height);
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void WindowCloseCallback(GLFWwindow* wptr){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceNewWindowCloseEvent();
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void KeyCallback(GLFWwindow* wptr, int key, int scancode, int action, int mods){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e;
    switch (action) {
        case GLFW_PRESS:
            e = LinceNewKeyPressedEvent(key, 0);
            break;
        case GLFW_RELEASE:
            e = LinceNewKeyReleasedEvent(key);
            break;
        case GLFW_REPEAT:
            e = LinceNewKeyPressedEvent(key, 1);
            break;
        default:
            LINCE_ASSERT(0, "Invalid KeyCallback action\n");
            break;
    }
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void CharCallback(GLFWwindow* wptr, unsigned int key_typed){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceNewKeyTypeEvent(key_typed);
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void MouseButtonCallback(GLFWwindow* wptr, int button, int action, int mods){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e;
    switch (action) {
        case GLFW_PRESS: {
            e = LinceNewMouseButtonPressedEvent(button);
            break;
        }
        case GLFW_RELEASE: {
            e = LinceNewMouseButtonReleasedEvent(button);
            break;
        }
        default:
            LINCE_ASSERT(0, "Invalid KeyCallback action\n");
            break;
    }
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void MouseScrolledCallback(GLFWwindow* wptr, double xoff, double yoff){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceNewMouseScrolledEvent(xoff, yoff);
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void MouseMovedCallback(GLFWwindow* wptr, double xpos, double ypos){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceNewMouseMovedEvent(xpos, ypos);
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}


// forward declare to call from CreateWindow
static void SetGLFWCallbacks(LinceWindow* w){
    GLFWwindow* window = w->handle;
    glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
    glfwSetWindowCloseCallback(window, WindowCloseCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, MouseScrolledCallback);
    glfwSetCursorPosCallback(window, MouseMovedCallback);
}