# Shaders


## LinceShader
- `unsigned int id`
- `char name[LINCE_NAME_MAX]`
- `char **uniform_names`
- `int *uniform_ids`
- `unsigned int uniform_count`

## LinceCreateShader
```c
LinceShader* LinceCreateShader(
	const char* name,
	const char* vertex_path,
	const char* fragment_path
)
```
## LinceCreateShaderFromSrc
```c
LinceShader* LinceCreateShaderFromSrc(
	const char* name,
	const char* vertex_src,
	const char* fragment_src
)
```
## LinceBindShader
```c
void LinceBindShader(LinceShader* shader)
```
## LinceUnbindShader
```c
void LinceUnbindShader(void)
```
## LinceDeleteShader
```c
void LinceDeleteShader(LinceShader* shader)
```
## LinceGetShaderUniformID
```c
int LinceGetShaderUniformID(LinceShader* shader, const char* name)
```
## LinceSetShaderUniformInt
```c
void LinceSetShaderUniformInt(
	LinceShader* sh,
	const char* name,
	int val
)
```
## LinceSetShaderUniformIntN
```c
void LinceSetShaderUniformIntN(
	LinceShader* sh,
	const char* name,
	int* arr,
	unsigned int count
)
```
## LinceSetShaderUniformFloat
```c
void LinceSetShaderUniformFloat(
	LinceShader* sh,
	const char* name,
	float val
)
```
## LinceSetShaderUniformVec2
```c
void LinceSetShaderUniformVec2(LinceShader* sh, const char* name, vec2 v)
```
## LinceSetShaderUniformVec3
```c
void LinceSetShaderUniformVec3(LinceShader* sh, const char* name, vec3 v);
```
## LinceSetShaderUniformVec4
```c
void LinceSetShaderUniformVec4(LinceShader* sh, const char* name, vec4 v);
```
## LinceSetShaderUniformMat3
```c
void LinceSetShaderUniformMat3(LinceShader* sh, const char* name, mat3 m)
```
## LinceSetShaderUniformMat4
```c
void LinceSetShaderUniformMat4(LinceShader* sh, const char* name, mat4 m);
```
