#include <glad/glad.h>

#include "lince/core/logger.h"
#include "lince/core/profiler.h"
#include "lince/utils/memory.h"
#include "lince/renderer/texture.h"


LinceTexture* LinceLoadTexture(const char* path, uint32_t flags){
	return LinceCreateTextureFromFile(path, flags);
}

LinceTexture* LinceCreateTextureFromFile(const char* path, uint32_t flags){
	LINCE_PROFILER_START(timer);

	LinceImageSetFlipVertical(flags & LinceTexture_FlipY);
	LinceImage image;
	if(!LinceLoadImage(&image, path)){
		return NULL;
	}
	if(flags & LinceTexture_WipeAlpha){
		LinceImageWipeAlphaChannel(&image);
	}

	LinceTexture* tex = LinceCreateTextureFromImage(&image, flags);
	if(!tex) return NULL;
	LinceDeleteImage(&image);

	LINCE_INFO("Created texture with size %ux%u from file '%s'",
		tex->width, tex->height, path);
	LINCE_PROFILER_END(timer);
	return tex;
}

LinceTexture* LinceCreateTextureFromImage(LinceImage* image, uint32_t flags){
	LinceTexture* texture = LinceCreateEmptyTexture(image->width, image->height);
	if(!texture) return NULL;

	// Apply flags
	// Note: flipping vertically can only be done before the image is loaded
	// Setting `LinceTexture_FlipY` will do nothing in this function.
	if(flags & LinceTexture_WipeAlpha){
		LinceImageWipeAlphaChannel(image);
	}

	LinceTextureSetData(texture, image->data);
	LINCE_INFO("Created texture from image with size %ux%u", image->width, image->height);
	return texture;
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
void LinceTextureSetData(LinceTexture* texture, unsigned char* data){
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