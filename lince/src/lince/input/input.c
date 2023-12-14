
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "input/input.h"
#include "lince/core/app.h"

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
    LincePoint pos;
    GLFWwindow* handle = LinceGetApp()->window->handle;
	glfwGetCursorPos(handle, &pos.x, &pos.y);
    return pos;
}

float LinceGetMouseX(){
    LinceGetMousePos().x;
}

float LinceGetMouseY(){
    LinceGetMousePos().y;
}
