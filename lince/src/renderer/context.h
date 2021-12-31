#ifndef LINCE_CONTEXT_H
#define LINCE_CONTEXT_H

typedef struct GLFWwindow GLFWwindow;

void GLContextInit(GLFWwindow* handle);
void GLContextSwapBuffers(GLFWwindow* handle);

#endif // LINCE_CONTEXT_H
