#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_IMPLEMENTATION
//#define NK_GLFW_GL4_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

#define NK_KEYSTATE_BASED_INPUT

#include "gui/nuklear.h"
//#include "gui/nuklear_glfw_gl4.h"
#include "gui/nuklear_glfw_gl3.h"
