
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "lince/input/input.h"
#include "lince/core/window.h"
#include "lince/core/logger.h"

static LinceWindow* active_window = NULL;

void LinceInputSetWindow(LinceWindow* window){
    active_window = window;
}

LinceBool LinceIsKeyPressed(int key){
    LINCE_ASSERT(active_window, "Attempting to read input without setting an active window");
    int state = glfwGetKey(active_window->handle, key);
    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

LinceBool LinceIsMouseButtonPressed(int button){
    LINCE_ASSERT(active_window, "Attempting to read input without setting an active window");
    int state = glfwGetMouseButton(active_window->handle, button);
	return (state == GLFW_PRESS);
}

LincePoint LinceGetMousePos(void) {
    LINCE_ASSERT(active_window, "Attempting to read input without setting an active window");
    double x, y;
	glfwGetCursorPos(active_window->handle, &x, &y);
    return (LincePoint){ (float)x, (float)y };
}

float LinceGetMouseX(){
    return LinceGetMousePos().x;
}

float LinceGetMouseY(){
    return LinceGetMousePos().y;
}
