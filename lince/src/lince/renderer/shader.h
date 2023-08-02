
#ifndef LINCE_SHADER_H
#define LINCE_SHADER_H

#include "lince/core/core.h"
#include "cglm/types.h"

#include "lince/containers/hashmap.h"

/** @struct shader */
typedef struct LinceShader {
	uint32_t id; 				///< OpenGL program id
	hashmap_t uniforms;   		///< Cache of uniform names and values
} LinceShader;

/** @brief Create and compile shader from source code files */
LinceShader* LinceCreateShader(
	const char* vertex_path,	///< Path to vertex shader source code
	const char* fragment_path	///< Path to fragment shader source code
);

/** @brief  Create and compile shader from source code as strings */
LinceShader* LinceCreateShaderFromSrc(
	const char* vertex_src,		///< Vertex source code as a char array
	const char* fragment_src	///< Fragment source code as a char array
);

/* Load shader from cached binary file */
// LinceShader* LinceLoadShaderFromCache(const char* name, const char* bin_path);

/* Save shader in memory to cache binary file */
// void LinceCacheShader(LinceShader* shader, const char* bin_path);

/** @brief Bind shader for use in rendering */
void LinceBindShader(LinceShader* shader);

/** @brief Unbinds the active shadern (if any) */
void LinceUnbindShader(void);

/** @brief Destroys and deallocates given shader */
void LinceDeleteShader(LinceShader* shader);

/** @brief Returns OpenGL ID of given uniform name, and -1 if it doesn't exist.
* Requires shader to be bound.
*/
int LinceGetShaderUniformID(LinceShader* shader, const char* uniform_name);

/** @brief Set an uniform of integer type.
* If the uniform was already set, its value is overwritten.
* Requires shader to be bound.
* @param sh shader for which to modify uniform.
* @param name Uniform string name
* @param val Value of the uniform
*/
void LinceSetShaderUniformInt(LinceShader* sh, const char* name, int val);

/** @brief Set an uniform with an array of integers.
* If the uniform was already set, its value is overwritten.
* Requires shader to be bound.
* @param sh shader for which to modify uniform.
* @param name Uniform string name
* @param arr Array of values
* @param count Number of values in the array
*/
void LinceSetShaderUniformIntN( LinceShader* sh, const char* name,
	int* arr, uint32_t count);

/** @brief Set an uniform with a float value.
* If the uniform was already set, its value is overwritten.
* Requires shader to be bound.
* @param sh shader for which to modify uniform.
* @param name Uniform string name
* @param val Value of the uniform
*/
void LinceSetShaderUniformFloat(LinceShader* sh, const char* name, float val);

/** @brief Set an uniform with a 2-float vector.
* If the uniform was already set, its value is overwritten.
* Requires shader to be bound.
* @param sh shader for which to modify uniform.
* @param name Uniform string name
* @param v Value of the uniform using cglm `vec2`.
*/
void LinceSetShaderUniformVec2(LinceShader* sh, const char* name, vec2 v);

/** @brief Set an uniform with a 3-float vector.
* If the uniform was already set, its value is overwritten.
* Requires shader to be bound.
* @param sh shader for which to modify uniform.
* @param name Uniform string name
* @param v Value of the uniform using cglm `vec3`.
*/
void LinceSetShaderUniformVec3(LinceShader* sh, const char* name, vec3 v);

/** @brief Set an uniform with a 4-float vector.
* If the uniform was already set, its value is overwritten.
* Requires shader to be bound.
* @param sh shader for which to modify uniform.
* @param name Uniform string name
* @param v Value of the uniform using cglm `vec4`.
*/
void LinceSetShaderUniformVec4(LinceShader* sh, const char* name, vec4 v);

/** @brief Set an uniform with a 3x3 float matrix.
* If the uniform was already set, its value is overwritten.
* Requires shader to be bound.
* @param sh shader for which to modify uniform.
* @param name Uniform string name
* @param v Value of the uniform using cglm `mat3`.
*/
void LinceSetShaderUniformMat3(LinceShader* sh, const char* name, mat3 m);

/** @brief Set an uniform with a 4x4 float matrix.
* If the uniform was already set, its value is overwritten.
* Requires shader to be bound.
* @param sh shader for which to modify uniform.
* @param name Uniform string name
* @param v Value of the uniform using cglm `mat4`.
*/
void LinceSetShaderUniformMat4(LinceShader* sh, const char* name, mat4 m);

#endif /* LINCE_SHADER_H */