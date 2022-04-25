#ifndef LINCE_CONTEXT_H
#define LINCE_CONTEXT_H

typedef struct GLFWwindow GLFWwindow; // forward declare

void LinceGLContextInit(GLFWwindow* handle);
void LinceGLContextSwapBuffers(GLFWwindow* handle);

#endif // LINCE_CONTEXT_H
