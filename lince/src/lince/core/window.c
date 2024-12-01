
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "lince/core/core.h"
#include "lince/core/window.h"
#include "lince/utils/memory.h"
#include "lince/core/logger.h"

#include "lince/event/event.h"
#include "lince/event/key_event.h"
#include "lince/event/mouse_event.h"
#include "lince/event/window_event.h"

/*
Returns the OpenGL string name for an error code.
Note: returning string literals is fine because they have static storage.
*/
static char const* LincGetGLErrorString(GLenum const err) {
  switch (err) {
    // opengl 2 errors (8)
    case GL_NO_ERROR:
      return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";

    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";

    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";

    // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // gles 2, 3 and gl 4 error are handled by the switch above
    default:
      return "Unknown Error";
  }
}

/*
Finds any raised OpenGL errors and if so, stops the program.
*/
static void LinceCheckGLErrors(){
    //return;
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        LINCE_ERROR("[OpenGL] Error %d: %s", err, LincGetGLErrorString(err));
        exit(-1);
    }
}


static void LinceGLFWErrorCallback(int error, const char* description) {
    LINCE_ERROR("[GLFW] Error %d: %s", error, description);
    exit(-1);
}

// forward declare to call from CreateWindow
static void LinceSetGLFWCallbacks();

/* Initialise OpenGL context */
static void LinceInitGLContext(GLFWwindow* handle){
    LINCE_ASSERT(handle, "Window handle is null");
    glfwMakeContextCurrent(handle);

    // Load GLAD
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LINCE_ASSERT(status, "[GLAD] Fatal error: failed to load!");
    
    // Debug info
    LINCE_INFO("GPU: %s", glGetString(GL_RENDERER));
    LINCE_INFO("Vendor: %s", glGetString(GL_VENDOR));
    LINCE_INFO("OpenGL Version: %s", glGetString(GL_VERSION));
}


/* Public API */

LinceWindow* LinceCreateWindow(uint32_t width, uint32_t height, const char* title){

    LINCE_ASSERT(glfwInit(), "Failed to initialise GLFW");
    
    /* Using OpenGL 4.0 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, LINCE_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, LINCE_GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(LinceGLFWErrorCallback);
    
    GLFWwindow* handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!handle) {
        glfwTerminate();
        LINCE_ASSERT(0, "Failed to create window");
    }
    LINCE_INFO("Window %dx%d created", width, height);
    LinceInitGLContext(handle);

    glfwSwapInterval(1); // activate vsync
    glViewport(0, 0, width, height);

    int glfw_major, glfw_minor, glfw_rev;
    glfwGetVersion(&glfw_major, &glfw_minor, &glfw_rev);
    LINCE_INFO("GLFW Version %d.%d.%d", glfw_major, glfw_minor, glfw_rev);

    LinceWindow* window = LinceAlloc(sizeof(LinceWindow));
    *window = (LinceWindow){
        .handle = handle,
        .height = height,
        .width = width,
        .initialised = 1,
        .title = "Lince Window",
        .event_callback = NULL
    };

    glfwSetWindowUserPointer((GLFWwindow*)window->handle, window);
    LinceSetGLFWCallbacks(window);

    return window;
}

uint32_t LinceShouldCloseWindow(LinceWindow* window){
    return glfwWindowShouldClose((GLFWwindow*)(window->handle));
}

void LinceUpdateWindow(LinceWindow* window){
    glfwSwapBuffers(window->handle);
	glfwPollEvents();
}

void LinceDestroyWindow(LinceWindow* window){
    glfwSetErrorCallback(NULL); // otherwise GLFW throws an error on shutdown
    if (window->initialised) glfwTerminate();
    if (window->handle) glfwDestroyWindow((GLFWwindow*)(window->handle));
	LinceFree(window);
}

void LinceSetMainEventCallback(LinceWindow* window, LinceEventCallbackFn func){
    window->event_callback = func;
}



/*
    ----- Event Callbacks -----
*/

static void WindowResizeCallback(GLFWwindow* wptr, int width, int height){
    glViewport(0, 0, width, height);
    
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    w->width = (uint32_t)width;
    w->height = (uint32_t)height;

    LinceEvent e = LinceNewWindowResizeEvent(width, height);
    if (w->event_callback) w->event_callback(&e);
    LinceEndEvent(&e);
}

static void WindowCloseCallback(GLFWwindow* wptr){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceNewWindowCloseEvent();
    if (w->event_callback) w->event_callback(&e);
    LinceEndEvent(&e);
}

static void KeyCallback(GLFWwindow* wptr, int key, int scancode, int action, int mods){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e;
    switch (action) {
        case GLFW_PRESS:
            e = LinceNewKeyPressEvent(key, 0);
            break;
        case GLFW_RELEASE:
            e = LinceNewKeyReleaseEvent(key);
            break;
        case GLFW_REPEAT:
            e = LinceNewKeyPressEvent(key, 1);
            break;
        default:
            LINCE_ASSERT(0, "Invalid KeyCallback action\n");
            break;
    }
    if (w->event_callback) w->event_callback(&e);
    LinceEndEvent(&e);
    LINCE_UNUSED(scancode);
    LINCE_UNUSED(mods);
}

static void CharCallback(GLFWwindow* wptr, uint32_t key_typed){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceNewKeyTypeEvent(key_typed);
    if (w->event_callback) w->event_callback(&e);
    LinceEndEvent(&e);
}

static void MouseButtonCallback(GLFWwindow* wptr, int button, int action, int mods){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e;
    switch (action) {
        case GLFW_PRESS: {
            e = LinceNewMousePressEvent(button);
            break;
        }
        case GLFW_RELEASE: {
            e = LinceNewMouseReleaseEvent(button);
            break;
        }
        default:
            LINCE_ASSERT(0, "Invalid KeyCallback action\n");
            break;
    }
    if (w->event_callback) w->event_callback(&e);
    LinceEndEvent(&e);
    LINCE_UNUSED(mods);
}

static void MouseScrolledCallback(GLFWwindow* wptr, double xoff, double yoff){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceNewMouseScrollEvent(xoff, yoff);
    if (w->event_callback) w->event_callback(&e);
    LinceEndEvent(&e);
}

static void MouseMovedCallback(GLFWwindow* wptr, double xpos, double ypos){
    LinceWindow* w = (LinceWindow*)glfwGetWindowUserPointer(wptr);
    LinceEvent e = LinceNewMouseMoveEvent(xpos, ypos);
    if (w->event_callback) w->event_callback(&e);
    LinceEndEvent(&e);
}


// forward declare to call from CreateWindow
static void LinceSetGLFWCallbacks(LinceWindow* w){
    GLFWwindow* window = w->handle;
    glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
    glfwSetWindowCloseCallback(window, WindowCloseCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, MouseScrolledCallback);
    glfwSetCursorPosCallback(window, MouseMovedCallback);
}