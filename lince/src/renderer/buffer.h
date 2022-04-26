#ifndef LINCE_BUFFER_H
#define LINCE_BUFFER_H

#include "core/core.h"

/* --- Buffer Element ---*/

/* Allowed data types for buffer elements */
typedef enum {
    LinceBufferType_None = 0,
    LinceBufferType_Bool, /* identical to int */
    LinceBufferType_Int,   LinceBufferType_Int2,   LinceBufferType_Int3,   LinceBufferType_Int4,
    LinceBufferType_Float, LinceBufferType_Float2, LinceBufferType_Float3, LinceBufferType_Float4,
    LinceBufferType_Mat3,  LinceBufferType_Mat4,
    LinceBufferType_Count /* number of buffer data types */
} LinceBufferType;

/* Describes one element of a buffer layout */
typedef struct LinceBufferElement {
    LinceBufferType type; /* Lince API data type */
    const char name[LINCE_NAME_MAX]; /* variable name in the shader */

    unsigned int gl_type; /* OpenGL API type */
    unsigned int comps; /* component count, e.g. Int4 has 4 components */
    unsigned int bytes; /* size in bytes */
    unsigned int offset; /* bytes from front of layout to this element */
} LinceBufferElement;

/* Returns details of a buffer type: component count, size, and OpenGL type */
LinceBufferElement LinceGetBufferTypeData(LinceBufferType type);

/* Copies buffer type details into provided buffer element */
void LinceSetupBufferElementData(LinceBufferElement* elem);


/* --- Vertex Buffer --- */

/* Stores OpenGL renderer ID */
typedef unsigned int LinceVertexBuffer;

/* Stores given data in new vertex buffer and returns renderer ID,
the supplied data may be empty (NULL) */
LinceVertexBuffer LinceCreateVertexBuffer(void* data, unsigned int size);

/* Populates an already existing buffer with data */
void LinceSetVertexBufferData(LinceVertexBuffer vb, void* data, unsigned int size);

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