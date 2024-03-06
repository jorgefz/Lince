#include <stb_image.h>
#include "lince/core/core.h"
#include "lince/utils/memory.h"
#include "lince/utils/image.h"


LinceImage* LinceLoadImage(LinceImage* image, const char* path){

    if(!image || !path) return NULL;

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
    stbi_set_flip_vertically_on_load(flip);
}