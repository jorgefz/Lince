#include "renderer/shader.h"
#include <glad/glad.h>



/* --- Static declarations --- */

/* Reads file and returns contents
as heap allocated string which must be freed */
static const char* LinceReadFile(const char* path);

/* Compiles a shader file from source, returns OpenGL ID */
static int LinceCompileShader(const char* source, int type);

/* --- Public API --- */

/* Create shader from paths to vertex and fragment shader source files */
LinceShader* LinceCreateShader(
	const char* vertex_path,
	const char* fragment_path
);

/* Create shader from source code of vertex and fragment shaders */
LinceShader* LinceCreateShaderFromSrc(
	const char* name,
	const char* vertex_src,
	const char* fragment_src
){
	LINCE_INFO(" Creating Shader '%s'", name);

	LinceShader* shader = calloc(1, sizeof(LinceShader));
	LINCE_ASSERT(shader,
		"Failed to allocate %d bytes", (int)sizeof(LinceShader));
	
	LINCE_ASSERT(strlen(name) < LINCE_NAME_MAX,
		"Shader name is too long: '%s'", name);
	memcpy(shader->name, name, strlen(name));
	shader->id = glCreateProgram();

	LINCE_INFO(" Compiling Vertex and Fragment Sources for '%s'", name);
	int vs, fs;
	vs = LinceCompileShader(vertex_src, GL_VERTEX_SHADER);
	fs = LinceCompileShader(fragment_src, GL_FRAGMENT_SHADER);

	LINCE_INFO(" Linking and Validating Shader '%s'", name);
	glAttachShader(shader->id, vs);
	glAttachShader(shader->id, fs);
	glLinkProgram(shader->id);
	glValidateProgram(shader->id);
	
	//compiled shader files no longer necessary
	glDeleteShader(vs);
	glDeleteShader(fs);

    return shader;
}

void LinceBindShader(LinceShader* shader){
	glUseProgram(shader->id);
}

void LinceUnbindShader(void){
	glUseProgram(0);
}

/* Provides string identifier of the shader */
const char* LinceGetShaderName(LinceShader* shader){
	return shader->name;
}

/* Destroys and deallocates given shader */
void LinceDeleteShader(LinceShader* shader){
	if(!shader) return;
	LINCE_INFO(" Deleting Shader '%s'", shader->name);
	if(shader->id > 0) glDeleteProgram(shader->id);
	free(shader);
}


/* --- Static functions --- */

int LinceCompileShader(const char* source, int type){

	int compile_sucess = LinceFalse;
	int id;

	id = glCreateShader(type);
	glShaderSource(id, 1, &source, NULL);
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &compile_sucess);

	if (compile_sucess != GL_TRUE) {
		// Retrieve GLSL compiler error message
		int length = 0;
		char msg[1000] = {0};
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		glGetShaderInfoLog(id, 1000, &length, &msg[0]);
		glDeleteShader(id);
		LINCE_ASSERT(0, " Failed to compile shader\n%s\n", msg);
	}
	
	return id;
}