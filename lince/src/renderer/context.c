
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/core.h"
#include "renderer/context.h"

void LinceGLContextInit(GLFWwindow* handle){
    LINCE_ASSERT(handle, "Window handle is null");
    glfwMakeContextCurrent(handle);

    // Load GLAD
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LINCE_ASSERT(status, "[GLAD] Fatal error: failed to load!");
    
    // Debug info - use
    LINCE_INFO("GPU: %s\n", glGetString(GL_RENDERER));
    LINCE_INFO("Vendor: %s\n", glGetString(GL_VENDOR));
    LINCE_INFO("OpenGL Version: %s\n", glGetString(GL_VERSION));
}

void LinceGLContextSwapBuffers(GLFWwindow* handle){
    glfwSwapBuffers(handle);
}