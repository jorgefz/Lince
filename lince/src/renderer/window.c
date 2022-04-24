
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core.h"

#include "event/event.h"
#include "event/keyEvent.h"
#include "event/mouseEvent.h"
#include "event/windowEvent.h"

#include "renderer/context.h"
#include "renderer/window.h"


static void GLFWErrorCallback(int error, const char* description) {
    fprintf(stderr, "GLFW ERROR %d -> ", error);
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
    fprintf(stderr, "GLFW Version %d.%d.%d\n", glfw_major, glfw_minor, glfw_rev);

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
    glfwSwapBuffers((GLFWwindow*)(window->handle));
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

    LinceEvent e = LinceEvent_NewWindowResizeEvent(width, height);
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void WindowCloseCallback(GLFWwindow* wptr){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceEvent_NewWindowCloseEvent();
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void KeyCallback(GLFWwindow* wptr, int key, int scancode, int action, int mods){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e;
    switch (action) {
        case GLFW_PRESS:
            e = LinceEvent_NewKeyPressedEvent(key, 0);
            break;
        case GLFW_RELEASE:
            e = LinceEvent_NewKeyReleasedEvent(key);
            break;
        case GLFW_REPEAT:
            e = LinceEvent_NewKeyPressedEvent(key, 1);
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
    LinceEvent e = LinceEvent_NewKeyTypeEvent(key_typed);
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void MouseButtonCallback(GLFWwindow* wptr, int button, int action, int mods){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e;
    switch (action) {
        case GLFW_PRESS: {
            e = LinceEvent_NewMouseButtonPressedEvent(button);
            break;
        }
        case GLFW_RELEASE: {
            e = LinceEvent_NewMouseButtonReleasedEvent(button);
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
    LinceEvent e = LinceEvent_NewMouseScrolledEvent(xoff, yoff);
    if (w->event_callback) w->event_callback(&e);
    LinceEvent_Destroy(&e);
}

static void MouseMovedCallback(GLFWwindow* wptr, double xpos, double ypos){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceEvent_NewMouseMovedEvent(xpos, ypos);
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