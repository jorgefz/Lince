/*
Include this header to use nuklear generic and glfw-specific functions
*/

#ifndef NUKLEAR_WRAPPER_H
#define NUKLEAR_WRAPPER_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#include "gui/nuklear.h"
//#include "gui/nuklear_glfw_gl4.h"
#include "gui/nuklear_glfw_gl3.h"


#endif