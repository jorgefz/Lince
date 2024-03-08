#ifndef LINCE_TEXTURE_H
#define LINCE_TEXTURE_H

#include "lince/core/core.h"
#include "lince/utils/image.h"

/** @enum LinceTextureFlags
* @brief Settings for loading a texture
*/
typedef enum LinceTextureFlags {
	LinceTexture_Default   = 0x0, ///< Explicit default settings
	LinceTexture_FlipY     = 0x1, ///< Flips texture vertically on load, setting the origin to the upper left corner.
	// LinceTexture_WipeAlpha = 0x2, ///< Removes transparency by setting the alpha channel to 1. 
} LinceTextureFlags;

/** @struct LinceTexture */
typedef struct LinceTexture {
	uint32_t id;               	///< OpenGL ID
	uint32_t width, height;    	///< 2D size
	int32_t data_format;     	///< Input format of texture file, e.g. RGBA
	int32_t internal_format; 	///< Output format of data in OpenGL buffer
} LinceTexture;

/** @brief Loads a texture from file.
* @deprecated Deprecated in favour of LinceCreateTextureFromFile.
* @param path Path to texture file
* @param flags Settings
*/
LinceTexture* LinceLoadTexture(const char* path, uint32_t flags);

/** @brief Loads a texture from file
* @param path Path to texture file
* @param flags Settings
*/
LinceTexture* LinceCreateTextureFromFile(const char* path, uint32_t flags);

/** @brief Loads a texture from a loaded image
* @param image Image loaded into memory 
* @param flags Settings
*/
LinceTexture* LinceCreateTextureFromImage(LinceImage* image, uint32_t flags);

/** @brief Creates empty texture buffer with given dimensions
* @param width  Width in pixels
* @param height Height in pixels
*/
LinceTexture* LinceCreateEmptyTexture(uint32_t width, uint32_t height);

/** @brief Provides custom data to an existing texture buffer.
 * @param texture Existing texture
 * @param data Image data to overwrite the texture
 * @note You can also modify the dimensions of the texture by changing
 * the width and height of the texture prior to calling this function.
 * @note This is an expensive operation.
*/
void LinceSetTextureData(LinceTexture* texture, unsigned char* data);

/** @brief Deallocates texture memory and destroys OpenGL texture object */
void LinceDeleteTexture(LinceTexture* texture);

/**@brief Binds the given texture to a slot (there are at least 16 slots) */
void LinceBindTexture(LinceTexture* texture, uint32_t slot);

// void LinceUnbindTexture(); // not necessary

#endif /* LINCE_TEXTURE_H */