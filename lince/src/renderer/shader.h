
#ifndef LINCE_SHADER_H
#define LINCE_SHADER_H

#include "core/core.h"

typedef struct LinceShader {
	unsigned int id; // opengl id
	char name[LINCE_NAME_MAX]; // name, normally based on source file
	
	char **uniform_names; // name in the shader code
	int *uniform_ids; // opengl uniform id
	unsigned int uniform_count; // length of uniform array
} LinceShader;

/* Create shader from paths to vertex and fragment shader source files */
LinceShader* LinceCreateShader(
	const char* name,
	const char* vertex_path,
	const char* fragment_path
);

/* Create shader from source code of vertex and fragment shaders */
LinceShader* LinceCreateShaderFromSrc(
	const char* name,
	const char* vertex_src,
	const char* fragment_src
);

void LinceBindShader(LinceShader* shader);
void LinceUnbindShader(void);

/* Provides string identifier of the shader */
const char* LinceGetShaderName(LinceShader* shader);

/* Destroys and deallocates given shader */
void LinceDeleteShader(LinceShader* shader);

/* Uniforms */
/* Returns OpenGL ID of given uniform name, and -1 if it doesn't exist */
int LinceGetShaderUniformID(LinceShader* shader, const char* name);

/* Add integer uniform */
void LinceSetShaderUniformInt(const char* name, int value);

/* Add integer array uniform */
void LinceSetShaderUniformIntN(
	const char* name, int* values, unsigned int count);

/* Add float uniform */
void LinceSetShaderUniformFloat(const char* name, float value);

/* Add float array uniform */
void LinceSetShaderUniformFloatN(
	const char* name, float* values, unsigned int count);

/* Add float matrix uniform */
void LinceSetShaderUniformMat(
	const char* name, float* values, int rows, int cols);


#endif /* LINCE_SHADER_H */