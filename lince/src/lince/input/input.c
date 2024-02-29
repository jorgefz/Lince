
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "input/input.h"
#include "lince/app/app.h"

LinceBool LinceIsKeyPressed(int key){
    GLFWwindow* handle = LinceGetApp()->window->handle;
    int state = glfwGetKey(handle, key);
    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

LinceBool LinceIsMouseButtonPressed(int button){
    GLFWwindow* handle = LinceGetApp()->window->handle;
    int state = glfwGetMouseButton(handle, button);
	return (state == GLFW_PRESS);
}

LincePoint LinceGetMousePos(void) {
    double x, y;
    GLFWwindow* handle = LinceGetApp()->window->handle;
	glfwGetCursorPos(handle, &x, &y);
    return (LincePoint){ (float)x, (float)y };
}

float LinceGetMouseX(){
    return LinceGetMousePos().x;
}

float LinceGetMouseY(){
    return LinceGetMousePos().y;
}
