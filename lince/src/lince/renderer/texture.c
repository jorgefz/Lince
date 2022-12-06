#include "core/profiler.h"
#include "renderer/texture.h"
#include <stb_image.h>
#include <glad/glad.h>

LinceTexture* LinceLoadTexture(const char* name, const char* path, uint32_t flags){
	LINCE_PROFILER_START(timer);
	LINCE_INFO(" Loading texture %s from '%s'", name, path);

	// sets buffer to store data starting from image top-left
	stbi_set_flip_vertically_on_load(flags & LinceTexture_FlipY);
	unsigned char* data = NULL;
	int width = 0, height = 0, channels = 0;
	LinceTexture *tex = NULL;

	// retrieve texture data
	data = stbi_load(path, &width, &height, &channels, 0);
	LINCE_ASSERT(data, " Failed to load texture '%s'", path);
	LINCE_ASSERT((width > 0) && (height > 0), " Empty texture '%s'", path);
	LINCE_ASSERT(channels == 4,
		" Error on image '%s'. Only 4-channel RGBA format supported", path);
	
	tex = LinceCreateEmptyTexture(name, (uint32_t)(width), (uint32_t)(height));
	LinceSetTextureData(tex, data);
	stbi_image_free(data);

	LINCE_INFO(" Loaded %dx%d texture %s", width, height, name);
	LINCE_PROFILER_END(timer);
	return tex;
}

/* DEPRECATED Use LinceLoadTexture 
- Creates texture from file
*/
LinceTexture* LinceCreateTexture(const char* name, const char* path){
	uint32_t flags = LinceTexture_FlipY;
	return LinceLoadTexture(name, path, flags);
}

/* Creates empty buffer with given dimensions */
LinceTexture* LinceCreateEmptyTexture(
	const char* name,
	uint32_t width,
	uint32_t height
) {
	LINCE_PROFILER_START(timer);
	// allocate texture data
	LinceTexture *tex = calloc(1, sizeof(LinceTexture));
	LINCE_ASSERT_ALLOC(tex, sizeof(LinceTexture));
	tex->width = width;
	tex->height = height;
	LINCE_ASSERT(strlen(name) < LINCE_NAME_MAX,
		" Texture name too long: %s", name);
	memcpy(tex->name, name, strlen(name));

	// default formats - only RGBA supported!!
	tex->internal_format = GL_RGBA8;
	tex->data_format = GL_RGBA;

	// create opengl object
	glCreateTextures(GL_TEXTURE_2D, 1, &tex->id);
	glTextureStorage2D(tex->id, 1, tex->internal_format, tex->width, tex->height);
	
	// Settings
	// interpolation by nearest pixel
	glTextureParameteri(tex->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(tex->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// for geometry larger than texture, repeat texture to fill out
	glTextureParameteri(tex->id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(tex->id, GL_TEXTURE_WRAP_T, GL_REPEAT);

	LINCE_PROFILER_END(timer);
	return tex;
}

/* Provides custom data to an existing texture buffer */
void LinceSetTextureData(LinceTexture* texture, unsigned char* data){
	LINCE_PROFILER_START(timer);
	glTextureSubImage2D(
		texture->id,          // OpenGL ID
		0, 0, 0,              // level, xoffset, yoffset
		texture->width,       // x size
		texture->height,      // y size
		texture->data_format, // e.g. GL_RGBA
		GL_UNSIGNED_BYTE,     // data type
		data                  // buffer
	);
	LINCE_PROFILER_END(timer);
}

/* Deallocates texture memory and destroys OpenGL texture object */
void LinceDeleteTexture(LinceTexture* texture){
	if(!texture) return;
	glDeleteTextures(1, &texture->id);
	free(texture);
}

/* Binds the given texture to a slot (there are at least 16 slots) */
void LinceBindTexture(LinceTexture* texture, uint32_t slot){
	glBindTextureUnit(slot, texture->id);
	/*
	The above API call is identical to the following:
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	Note: don't do 'GL_TEXTURE0 + slot' on glBindTextureUnit,
		rather pass slot value directly.
	*/
}