
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "lince/input/input.h"
#include "lince/core/logger.h"
#include "lince/app/app.h" // OK if its only included in the implementation file

LinceBool LinceIsKeyPressed(int key){
    int state = glfwGetKey(LinceGetApp()->window->handle, key);
    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

LinceBool LinceIsMouseButtonPressed(int button){
    int state = glfwGetMouseButton(LinceGetApp()->window->handle, button);
	return (state == GLFW_PRESS);
}

LincePoint LinceGetMousePos(void) {
    double x, y;
	glfwGetCursorPos(LinceGetApp()->window->handle, &x, &y);
    return (LincePoint){ (float)x, (float)y };
}

float LinceGetMouseX(){
    return LinceGetMousePos().x;
}

float LinceGetMouseY(){
    return LinceGetMousePos().y;
}
