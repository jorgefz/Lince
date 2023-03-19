#ifndef LINCE_BUFFER_H
#define LINCE_BUFFER_H

#include "lince/core/core.h"

/** @file buffer.h
    Vertex buffers store information on the properties of vertices
    rendered on the screen.
    This could include its position, color, texture coordinates, etc.

    A buffer element represents one of these properties.
    For instance, position could be represented as an array of two floats (x and y).
    Color, an array of four floats (r, g, b, and alpha).
    A buffer element also tells you what its place is
    within the vertex buffer layout.
*/

/** @brief Allowed data types for buffer elements */
typedef enum {
    LinceBufferType_None = 0,
    LinceBufferType_Bool, /**> boolean - identical to int */
    LinceBufferType_Int,   LinceBufferType_Int2,   LinceBufferType_Int3,   LinceBufferType_Int4,
    LinceBufferType_Float, LinceBufferType_Float2, LinceBufferType_Float3, LinceBufferType_Float4,
    LinceBufferType_Mat3,  LinceBufferType_Mat4, /**> 3x3 and 4x4 matrices of floats */
    LinceBufferType_Count /**> number of buffer data types defined above */
} LinceBufferType;

/** @brief Describes one element of a buffer layout */
typedef struct LinceBufferElement {
    LinceBufferType type;            /**> Lince API data type */
    const char name[LINCE_NAME_MAX]; /**> Variable name in the shader */

    uint32_t gl_type;   /**> Data type defined with OpenGL codes */
    uint32_t comps;     /**> Component count, e.g. Int4 has 4 components */
    uint32_t bytes;     /**> Size in bytes */
    uint32_t offset;    /**> Bytes from the front of vertex layout */
} LinceBufferElement;

/** @brief Returns details of a buffer type: component count, size, and OpenGL type.
* Turns `LinceBufferType` into `LinceBufferElement`.
* For instance, the type Float2 has size float*2, has two components, and OpenGL type GL_FLOAT.
*/
LinceBufferElement LinceGetBufferTypeData(LinceBufferType type);

/** @brief Copies buffer type details into provided buffer element */
void LinceSetupBufferElementData(LinceBufferElement* elem);


/* --- Vertex Buffer --- */

/** @typedef Stores OpenGL renderer ID */
typedef uint32_t LinceVertexBuffer;

/** @brief Stores given data in new vertex buffer and returns renderer ID.
The supplied data may be empty (NULL). */
LinceVertexBuffer LinceCreateVertexBuffer(void* data, uint32_t size);

/** @brief Populates an already existing buffer with data */
void LinceSetVertexBufferData(LinceVertexBuffer vb, void* data, uint32_t size);

/** @brief Binds a vertex buffer for being rendered */
void LinceBindVertexBuffer(LinceVertexBuffer vb);

/** @brief Unbinds the current bound vertex buffer (if any) */
void LinceUnbindVertexBuffer();

/** @brief Removes a vertex buffer from memory */
void LinceDeleteVertexBuffer(LinceVertexBuffer vb);


/* --- Index Buffer --- */

/** An index buffer stores the order in which vertices are drawn to the screen.
* This is helpful because OpenGL only renders triangles to screen,
* so you have to break-up more complex shapes into triangles.
*/
typedef struct LinceIndexBuffer {
    uint32_t id;     //> OpenGL renderer ID
    uint32_t count;  //> number of indices
} LinceIndexBuffer;

/** Create an index buffer with a given set of vertex indices */
LinceIndexBuffer LinceCreateIndexBuffer(uint32_t* data, uint32_t count);

/** Binds an index buffer for rendering */
void LinceBindIndexBuffer(LinceIndexBuffer ib);

/** Unbinds any index buffer currently bound (if any) */
void LinceUnbindIndexBuffer();

/** Removes an index buffer from memory */
void LinceDeleteIndexBuffer(LinceIndexBuffer ib);


#endif // LINCE_BUFFER_H