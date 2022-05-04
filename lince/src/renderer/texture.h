#ifndef LINCE_TEXTURE_H
#define LINCE_TEXTURE_H

#include "core/core.h"

typedef struct LinceTexture {
	uint32_t id;               // OpenGL ID
	uint32_t width, height;    // dimension of 2D texture
	char name[LINCE_NAME_MAX]; // unique string identifier
	int32_t data_format;     // format of data in OpenGL buffer
	int32_t internal_format; // format of texture file, e.g. RGBA
} LinceTexture;

/* Creates texture from file */
LinceTexture* LinceCreateTexture(const char* path);

/* Creates empty buffer with given dimensions */
LinceTexture* LinceCreateEmptyTexture(uint32_t width, uint32_t height);

/* Provides custom data to an existing texture buffer */
void LinceSetTextureData(LinceTexture* texture, unsigned char* data);

/* Deallocates texture memory and destroys OpenGL texture object */
void LinceDeleteTexture(LinceTexture* texture);

/* Binds the given texture to a slot (there are at least 16 slots) */
void LinceBindTexture(LinceTexture* texture, uint32_t slot);

// void LinceUnbindTexture(); // not necessary

#endif /* LINCE_TEXTURE_H */