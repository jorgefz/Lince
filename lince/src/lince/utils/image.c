#include <stb_image.h>
#include "lince/core/core.h"
#include "lince/core/logger.h"
#include "lince/utils/memory.h"
#include "lince/utils/image.h"


void* LinceLoadImageAsset(string_t path, void* args){
    (void)args;
    return LinceLoadImage(path.str);
}

void LinceUnloadImageAsset(void* img){
    LinceDeleteImage(img);
}

LinceImage* LinceLoadImage(const char* path){
    
    if(!path) return NULL;

    LinceImage* image = LinceAlloc(sizeof(LinceImage));
    if(!image) return NULL;

    static int rgba_channels = 4;
    int width, height;
    int channels; // unused, forced to 4 channels (RGBA)

    // Already heap-allocated by the library
    image->data = stbi_load(path, &width, &height, &channels, rgba_channels);

    // Error checking
    if(!image->data){
        LINCE_ERROR("Failed to load image '%s': %s", path, stbi_failure_reason());
        LinceDeleteImage(image);
        return NULL;
    } else if (width <= 0 || height <= 0){
        LINCE_ERROR("Loaded image is empty: '%s'", path);
        LinceDeleteImage(image);
        return NULL;
    }

    image->width    = (uint32_t)width;
    image->height   = (uint32_t)height;
    image->channels = (uint32_t)rgba_channels;
    LINCE_INFO("Loaded image of size %ux%u from '%s'", image->width, image->height, path);
    
    return image;
}

void LinceDeleteImage(LinceImage* image){
    if(!image) return;
    if(image->data){
        stbi_image_free(image->data);
        LINCE_INFO("Unloaded image of size %ux%u", image->width, image->height);
    }
    *image = (LinceImage){0};
}

uint8_t* LinceImageGetPixel(LinceImage* image, uint32_t x, uint32_t y){
    
    // Error checking
    if(!image || !image->data) return NULL;
    if(x >= image->height || y >= image->width){
        LINCE_ERROR("Image pixel coordinates (%u,%u) out of bounds (%u,%u)",
            x, y, image->width-1, image->height-1
        );
        return NULL;
    }

    return image->data + (y*image->width + x) * image->channels;
}

uint8_t* LinceImageSetPixel(LinceImage* image, uint32_t x, uint32_t y, uint32_t rgba){
    uint8_t* pixel = LinceImageGetPixel(image, x, y);
    if(!pixel) return NULL;
    *((uint32_t*)(void*)pixel) = rgba;
    return pixel;
}

void LinceImageSetFlipVertical(LinceBool flip){
    /*
    The reason why the boolean is flipped before being
    passed to `stbi_set_flip_vertically_on_load` is because
    stb_image's default origin is on the upper left corner,
    and this switch changes it to lower left corner.

    The Lince default is the opposite, like OpenGL:
    the origin is on the lower left corner,
    and this function sets it to the upper left corner.
    */
    stbi_set_flip_vertically_on_load(!flip);
}


/** @brief Clears the alpha channel to be fully opaque
 * @param image Image to make opaque
**/ 
void LinceImageWipeAlphaChannel(LinceImage* image){
	for(uint32_t i = 0; i != image->width; ++i){
		for(uint32_t j = 0; j != image->height; ++j){
			uint8_t* pixel = LinceImageGetPixel(image, i, j);
			pixel[3] = (uint8_t)(-1); // underflows to max
		}
	}
}