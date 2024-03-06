#include "core/profiler.h"
#include "utils/memory.h"
#include "renderer/texture.h"
#include "lince/utils/image.h"
#include <glad/glad.h>


LinceTexture* LinceLoadTexture(const char* path, uint32_t flags){
	LINCE_PROFILER_START(timer);

	LinceImageSetFlipVertical(flags & LinceTexture_FlipY);

	LinceImage image;
	void* result = LinceLoadImage(&image, path);
	LINCE_ASSERT(result, "Failed to load texture from '%s'", path);
	if(!result) return NULL;

	LinceTexture* tex = LinceCreateEmptyTexture(image.width, image.height);
	if(!tex) return NULL;
	LinceSetTextureData(tex, image.data);
	LinceDeleteImage(&image);

	LINCE_INFO("Created texture with size %ux%u", tex->width, tex->height);
	LINCE_PROFILER_END(timer);
	return tex;
}

/* Creates empty buffer with given dimensions */
LinceTexture* LinceCreateEmptyTexture(uint32_t width, uint32_t height){
	LINCE_PROFILER_START(timer);
	
	LinceTexture *tex = LinceCalloc(sizeof(LinceTexture));
	LINCE_ASSERT_ALLOC(tex, sizeof(LinceTexture));
	if(!tex) return NULL;

	tex->width = width;
	tex->height = height;

	// Default formats - only RGBA supported!!
	tex->internal_format = GL_RGBA8;
	tex->data_format = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &tex->id);
	glTextureStorage2D(tex->id, 1, tex->internal_format, tex->width, tex->height);
	
	// -- Settings
	// Interpolation by nearest pixel
	glTextureParameteri(tex->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(tex->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// For geometry larger than texture, repeat texture to fill out
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
	LinceFree(texture);
}

/* Binds the given texture to a slot (there are at least 16 slots) */
void LinceBindTexture(LinceTexture* texture, uint32_t slot){
	/*
	The function below is identical to the following:
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	Note: don't do 'GL_TEXTURE0 + slot' on glBindTextureUnit,
		rather pass slot value directly.
	*/
	glBindTextureUnit(slot, texture->id);
}