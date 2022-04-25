
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/input.h"
#include "core/app.h"

unsigned int LinceIsKeyPressed(int key){
    GLFWwindow* handle = LinceGetAppState()->window->handle;
    int state = glfwGetKey(handle, key);
    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

unsigned int LinceIsMouseButtonPressed(int button){
    GLFWwindow* handle = LinceGetAppState()->window->handle;
    int state = glfwGetMouseButton(handle, button);
	return (state == GLFW_PRESS);
}

void LinceGetMousePos(float* xpos, float* ypos){
    double x, y;
    GLFWwindow* handle = LinceGetAppState()->window->handle;
	glfwGetCursorPos(handle, &x, &y);
    if (xpos) *xpos = (float)x;
    if (ypos) *ypos = (float)y;
}

float LinceGetMouseX(){
    float x;
    LinceGetMousePos(&x, NULL);
    return x;
}

float LinceGetMouseY(){
    float y;
    LinceGetMousePos(NULL, &y);
    return y;
}
