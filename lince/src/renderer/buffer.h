#ifndef LINCE_BUFFER_H
#define LINCE_BUFFER_H

// enum GLSLTypes;

typedef enum {
    BufferElement_None = 0,
    BufferElement_Bool,
    BufferElement_Int, BufferElement_Int2, BufferElement_Int3, BufferElement_Int4,
    BufferElement_Float, BufferElement_Float2, BufferElement_Float3, BufferElement_Float4,
    BufferElement_Mat3, BufferElement_Mat4
} BufferElement;

typedef struct BufferLayout {
    unsigned int elems; // number of buffer elements
    unsigned int comp; // number of components /(e.g. Int4 counts as 4)
    unsigned int bytes; // size in bytes
    BufferElement* elements; // array of ints
} BufferLayout;

unsigned int BufferElementBytes(BufferElement elem);
BufferLayout* CreateBufferLayout(unsigned int count, ...);


#endif // LINCE_BUFFER_H