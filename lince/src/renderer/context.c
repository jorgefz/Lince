
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core.h"
#include "renderer/context.h"

void GLContextInit(GLFWwindow* handle){
    LINCE_ASSERT(handle, "Window handle is null");
    glfwMakeContextCurrent(handle);

    // Load GLAD
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LINCE_ASSERT(status, "[GLAD] Fatal error: failed to load!");
    
    // Debug info
    fprintf(stderr, "GPU: %s\n", glGetString(GL_RENDERER));
    fprintf(stderr, "Vendor: %s\n", glGetString(GL_VENDOR));
    fprintf(stderr, "OpenGL Version: %s\n", glGetString(GL_VERSION));
}

void GLContextSwapBuffers(GLFWwindow* handle){
    glfwSwapBuffers(handle);
}