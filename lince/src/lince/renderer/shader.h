
#ifndef LINCE_SHADER_H
#define LINCE_SHADER_H

#include "lince/core/core.h"
#include "cglm/types.h"

#include "lince/containers/hashmap.h"


typedef struct LinceShader {
	uint32_t id; // opengl id
	char name[LINCE_NAME_MAX]; // name, normally based on source file
	hashmap_t uniforms;   // cache of uniforms
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

/*
Uniform functions.
Ensure a shader is bound before using these functions
*/
/* Returns OpenGL ID of given uniform name, and -1 if it doesn't exist */
int LinceGetShaderUniformID(LinceShader* shader, const char* name);

/* Set integer uniform */
void LinceSetShaderUniformInt(LinceShader* sh, const char* name, int val);

/* Set integer array uniform */
void LinceSetShaderUniformIntN( LinceShader* sh, const char* name,
	int* arr, uint32_t count);

/* Set float uniform */
void LinceSetShaderUniformFloat(LinceShader* sh, const char* name, float val);

/* Set vec2 uniform */
void LinceSetShaderUniformVec2(LinceShader* sh, const char* name, vec2 v);

/* Set vec3 uniform */
void LinceSetShaderUniformVec3(LinceShader* sh, const char* name, vec3 v);

/* Set vec4 uniform */
void LinceSetShaderUniformVec4(LinceShader* sh, const char* name, vec4 v);

/* Set mat3 uniform */
void LinceSetShaderUniformMat3(LinceShader* sh, const char* name, mat3 m);

/* Set mat4 uniform */
void LinceSetShaderUniformMat4(LinceShader* sh, const char* name, mat4 m);

#endif /* LINCE_SHADER_H */