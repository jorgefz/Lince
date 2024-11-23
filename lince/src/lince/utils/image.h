#ifndef LINCE_IMAGE_H
#define LINCE_IMAGE_H

#include "lince/core/core.h"
#include "lince/containers/str.h"

/** @struct LinceImage
 * @brief Buffer for storing an image loaded from disk.
 * Wrapper for stb_image by Sean Barret (https://github.com/nothings).
*/
typedef struct LinceImage {
    uint32_t width;     ///< Image width in pixels
    uint32_t height;    ///< Image height in pixels
    uint32_t channels;  ///< Number of channels, fixed at 4 (RGBA)
    uint8_t* data;      ///< Image data ordered as Y scanlines of X pixels, starting from top-left.
} LinceImage;

/** @brief Callback for asset cache to load image from disk */
void* LinceLoadImageAsset(string_t path, void* args);

/** @brief Callback for asset cache to unload image */
void LinceUnloadImageAsset(void* img);

/** @brief Loads an image from disk.
 * @param path filename of the image to load
 * @returns loaded image if successful, or NULL otherwise.
*/
LinceImage* LinceLoadImage(const char* path);

/** @brief Unloads (frees) an image from memory.
 * @param image Image to free
*/
void LinceDeleteImage(LinceImage* image);

/** @brief Returns a pointer to the pixel at location (x,y)
 * @param image Loaded image to probe
 * @param x X-coordinate of the pixel
 * @param y Y-coordinate of the pixel
 * @returns pointer to the first byte of the requested pixel
*/
uint8_t* LinceImageGetPixel(LinceImage* image, uint32_t x, uint32_t y);

/** @brief Returns a pointer to the pixel at location (x,y)
 * @param image Loaded image to modify
 * @param x X-coordinate of the pixel
 * @param y Y-coordinate of the pixel
 * @param rgba Color value to store in the pixel
 * @returns pointer to the first byte of the modified pixel
*/
uint8_t* LinceImageSetPixel(LinceImage* image, uint32_t x, uint32_t y, uint32_t rgba);

/** @brief Flip new loaded images vertically on load,
 * so that the origin lies on the upper left corner.
 * @note The default behaviour of stb_image is to have the origin
 * on the upper left corner. The default has been changed to the lower left corner,
 * like in OpenGL.
*/
void LinceImageSetFlipVertical(LinceBool flip);

/** @brief Clears the alpha channel to be fully opaque
 * @param image Image to make opaque
**/ 
void LinceImageWipeAlphaChannel(LinceImage* image);

#endif /* LINCE_IMAGE_H */