#include "renderer/shader.h"
#include <string.h>
#include <glad/glad.h>
//#include <cglm>



/* --- Static declarations --- */

/* Reads file and returns contents
as heap allocated string which must be freed */
static char* LinceReadFile(const char* path);

/* Compiles a shader file from source, returns OpenGL ID */
static int LinceCompileShader(const char* source, int type);


/* --- Public API --- */

/* Create shader from paths to vertex and fragment shader source files */
LinceShader* LinceCreateShader(
	const char* name,
	const char* vertex_path,
	const char* fragment_path
){
	LINCE_INFO(" Creating Shader '%s'", name);
	LinceShader* shader;
	char *vsrc, *fsrc;

	vsrc = LinceReadFile(vertex_path);
	fsrc = LinceReadFile(fragment_path);
	shader = LinceCreateShaderFromSrc(name, vsrc, fsrc);

	free(vsrc);
	free(fsrc);

	return shader;
}

/* Create shader from source code of vertex and fragment shaders */
LinceShader* LinceCreateShaderFromSrc(
	const char* name,
	const char* vertex_src,
	const char* fragment_src
){
	LINCE_INFO(" Creating Shader '%s' From Source", name);

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
	
	//compiled shader files are no longer necessary
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
	if(shader->uniform_names){
		for(int i=0; i!=(int)shader->uniform_count; ++i){
			if(!shader->uniform_names[i]) continue;
			free(shader->uniform_names[i]);
		}
		free(shader->uniform_names);
	}
	if(shader->uniform_ids) free(shader->uniform_ids);
	free(shader);
}

int LinceGetShaderUniformID(LinceShader* shader, const char* name){
	/* Refactor this to cache the uniform locations */
	if(!shader || !name) return -1;
	int location =  glGetUniformLocation(shader->id, name);
	if(location < 0) LINCE_INFO(" Uniform '%s' not found in shader '%s'",
								name, shader->name);
	return location;
	/*
	LINCE_ASSERT(
		strlen(name) < LINCE_NAME_MAX,
		" Shader uniform name %d bytes too long '%s'",
		(int)strlen(name) - LINCE_NAME_MAX, name
	);
	
	// locate uniform in cache, return if sucessful
	for(int i=0; i!=(int)shader->uniform_count; ++i){
		if(strcmp(name, shader->uniform_names[i]) == 0){
			return shader->uniform_ids[i];
		}
	}

	// locate uniform in shader
	int location = glGetUniformLocation(shader->id, name);
	if(location < 0){
		LINCE_INFO(" Shader Uniform '%s' does not exist", name);
		return -1;
	}

	// append uniform to cache
	shader->uniform_names = realloc(
		shader->uniform_names,
		shader->uniform_count + 1
	);
	LINCE_ASSERT_ALLOC(shader->uniform_names, shader->uniform_count + 1);

	// extend name list
	char** uname = &shader->uniform_names[shader->uniform_count];
	*uname = calloc( LINCE_NAME_MAX, sizeof(char) );
	LINCE_ASSERT_ALLOC(*uname, LINCE_NAME_MAX*sizeof(char));

	memcpy(*uname, name, strlen(name));

	// extend ID list
	int* loc = &shader->uniform_ids[shader->uniform_count];
	loc = malloc( sizeof(int) );
	LINCE_ASSERT_ALLOC(loc, sizeof(int));
	*loc = location;

	shader->uniform_count++;
	return location;
	*/
}

/* Set integer uniform */
void LinceSetShaderUniformInt(LinceShader* sh, const char* name, int val){
	int loc = LinceGetShaderUniformID(sh, name);
	glUniform1i(loc, val);
}

/* Set integer array uniform */
void LinceSetShaderUniformIntN(
	LinceShader* sh, const char* name,
	int* arr, unsigned int count
) {
	int loc = LinceGetShaderUniformID(sh, name);
	glUniform1iv(loc, count, arr);
}

/* Set float uniform */
void LinceSetShaderUniformFloat(LinceShader* sh, const char* name, float val){
	int loc = LinceGetShaderUniformID(sh, name);
	glUniform1f(loc, val);
}

/* Set vec2 uniform */
void LinceSetShaderUniformVec2(LinceShader* sh, const char* name, vec2 v){
	int loc = LinceGetShaderUniformID(sh, name);
	glUniform2f(loc, v[0], v[1]);
}

/* Set vec3 uniform */
void LinceSetShaderUniformVec3(LinceShader* sh, const char* name, vec3 v){
	int loc = LinceGetShaderUniformID(sh, name);
	glUniform3f(loc, v[0], v[1], v[2]);
}

/* Set vec4 uniform */
void LinceSetShaderUniformVec4(LinceShader* sh, const char* name, vec4 v){
	int loc = LinceGetShaderUniformID(sh, name);
	glUniform4f(loc, v[0], v[1], v[2], v[3]);
}

/* Set mat3 uniform */
void LinceSetShaderUniformMat3(LinceShader* sh, const char* name, mat3 m){
	int loc = LinceGetShaderUniformID(sh, name);
	glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

/* Set mat4 uniform */
void LinceSetShaderUniformMat4(LinceShader* sh, const char* name, mat4 m){
	int loc = LinceGetShaderUniformID(sh, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}


/* --- Static functions --- */

static char* LinceReadFile(const char* path){
	LINCE_INFO(" Reading File '%s'", path);
	FILE* handle = fopen(path, "r");
	LINCE_ASSERT(handle, " Failed to open file '%s'", path);

	/* Get file length */
	fseek(handle, 0, SEEK_END);
	size_t size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	LINCE_ASSERT(size > 0, " Empty file '%s'", path);

	char* source = calloc(size+1, sizeof(char));
	LINCE_ASSERT_ALLOC(source, size+1);
	fread(source, size, 1, handle); // load file data into buffer
	source[size] = '\0'; // ensure x2 last character is terminator

	fclose(handle);
	return source;
}


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