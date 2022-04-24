
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "input.h"

#include "app.h"

unsigned int LinceInput_IsKeyPressed(int key){
    GLFWwindow* handle = LinceGetAppState()->window->handle;
    int state = glfwGetKey(handle, key);
    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

unsigned int LinceInput_IsMouseButtonPressed(int button){
    GLFWwindow* handle = LinceGetAppState()->window->handle;
    int state = glfwGetMouseButton(handle, button);
	return (state == GLFW_PRESS);
}

void LinceInput_GetMousePos(float* xpos, float* ypos){
    double x, y;
    GLFWwindow* handle = LinceGetAppState()->window->handle;
	glfwGetCursorPos(handle, &x, &y);
    if (xpos) *xpos = (float)x;
    if (ypos) *ypos = (float)y;
}

float LinceInput_GetMouseX(){
    float x;
    LinceInput_GetMousePos(&x, NULL);
    return x;
}

float LinceInput_GetMouseY(){
    float y;
    LinceInput_GetMousePos(NULL, &y);
    return y;
}
