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
## LinceCreateEmptyTexture
```c
LinceTexture* LinceCreateEmptyTexture(
	const char* name, 
	uint32_t width,
	uint32_t height
)
```
## LinceSetTextureData
```c
void LinceSetTextureData(LinceTexture* texture, unsigned char* data)
```
## LinceDeleteTexture
```c
void LinceDeleteTexture(LinceTexture* texture)
```
## LinceBindTexture
```c
void LinceBindTexture(LinceTexture* texture, uint32_t slot)
```
