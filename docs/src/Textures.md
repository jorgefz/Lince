# Textures

## LinceTexture
- `uint32_t id`
- `uint32_t width`
- `uint32_t height`
- `char name[LINCE_NAME_MAX]`
- `int32_t data_format`
- `int32_t internal_format`

## LinceCreateTexture
```c
LinceTexture* LinceCreateTexture(const char* name, const char* path)
```
Loads a texture from a file at 'path', using a string 'name' as ID.

## LinceCreateEmptyTexture
```c
LinceTexture* LinceCreateEmptyTexture(
	const char* name, 
	uint32_t width,
	uint32_t height
)
```
Creates an empty texture of given size.

## LinceSetTextureData
```c
void LinceSetTextureData(LinceTexture* texture, unsigned char* data)
```
Provides byte data to a texture, useful for empty textures.

## LinceDeleteTexture
```c
void LinceDeleteTexture(LinceTexture* texture)
```
Frees texture data and deletes corresponding OpenGL object.

## LinceBindTexture
```c
void LinceBindTexture(LinceTexture* texture, uint32_t slot)
```
Binds a texture to a slot so that it may be used in rendering.
Note that the total number of slots vary depending on platform, but is usually 32 on modern platforms.
