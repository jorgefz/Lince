#ifndef STB_IMAGE_ALLOC_H
#define STB_IMAGE_ALLOC_H


void stbi_set_alloc(
    void* (*umalloc)(size_t),
    void* (*urealloc)(void*, size_t),
    void  (*ufree)(void*)
);


#endif /* STB_IMAGE_ALLOC_H */