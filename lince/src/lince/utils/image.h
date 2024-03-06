#ifndef LINCE_IMAGE_H
#define LINCE_IMAGE_H

#include "lince/core/core.h"

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

/** @brief Loads an image from disk.
 * @param image 
 * @param path filename of the image to load
 * @returns input image if successful, or NULL otherwise.
*/
LinceImage* LinceLoadImage(LinceImage* image, const char* path);

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

/** @brief Flip new loaded images vertically on load, so that the origin lies on the bottom left corner */
void LinceImageSetFlipVertical(LinceBool flip);

#endif /* LINCE_IMAGE_H */