#ifndef LINCE_BUFFER_H
#define LINCE_BUFFER_H

#include "core/core.h"

/* --- Buffer Element ---*/

/* Allowed data types for buffer elements */
typedef enum {
    LinceBufferType_None = 0,
    LinceBufferType_Bool,
    LinceBufferType_Int,   LinceBufferType_Int2,   LinceBufferType_Int3,   LinceBufferType_Int4,
    LinceBufferType_Float, LinceBufferType_Float2, LinceBufferType_Float3, LinceBufferType_Float4,
    LinceBufferType_Mat3,  LinceBufferType_Mat4,
    LinceBufferType_Count /* number of buffer data types */
} LinceBufferType;

/* Describes one element of a buffer layout */
typedef struct LinceBufferElement {
    LinceBufferType type; // BufferType
    const char name[LINCE_STR_MAX]; // name in GLSL code

    unsigned int gl_type; // OpenGL API type
    unsigned int comps; // number of components
    unsigned int bytes; // size in bytes
    // offset, ...
} LinceBufferElement;


/* --- Vertex Buffer --- */

/* Stores OpenGL renderer ID */
typedef unsigned int LinceVertexBuffer;

/* Stores given data in new vertex buffer and returns renderer ID,
the supplied data may be empty (NULL) */
LinceVertexBuffer LinceCreateVertexBuffer(float* data, unsigned int size);

/* Populates an already existing buffer with data */
void LinceSetVertexBufferData(LinceVertexBuffer vb, float* data, unsigned int size);

void LinceBindVertexBuffer(LinceVertexBuffer vb);
void LinceUnbindVertexBuffer();
void LinceDeleteVertexBuffer(LinceVertexBuffer vb);


/* --- Index Buffer --- */

typedef struct LinceIndexBuffer {
    unsigned int id; // OpenGL renderer ID
    unsigned int count; // number of indices
} LinceIndexBuffer;

LinceIndexBuffer LinceCreateIndexBuffer(unsigned int* data, unsigned int count);
void LinceBindIndexBuffer(LinceIndexBuffer ib);
void LinceUnbindIndexBuffer();
void LinceDeleteIndexBuffer(LinceIndexBuffer ib);


#endif // LINCE_BUFFER_H