#include "core/profiler.h"
#include <GLFW/glfw3.h>

double LinceGetTimeMillisec(void){
	return (glfwGetTime() * 1000.0);
}

