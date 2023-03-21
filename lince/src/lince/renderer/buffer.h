/** @file buffer.h
* Vertex buffers store information on the properties of vertices
* rendered on the screen.
* This could include its position, color, texture coordinates, etc.
*
* A buffer element represents one of these properties.
* For instance, position could be represented as an array of two floats (x and y).
* Color, an array of four floats (r, g, b, and alpha).
* A buffer element also tells you what its place is
* within the vertex buffer layout.
*/

#ifndef LINCE_BUFFER_H
#define LINCE_BUFFER_H

#include "lince/core/core.h"

/** @enum LinceBufferType
* @brief Allowed data types for buffer elements
| Enum                   | Value | Components | Bytes |
| ---------------------- | ----- | ---------- | ----- |
| LinceBufferType_None   |   0   | 		0	  |   0   |
| LinceBufferType_Bool   |   1   | 		1	  |   4   |
| LinceBufferType_Int    |   2   | 		1	  |   4   |
| LinceBufferType_Int2   |   3   | 		2	  |   8   |
| LinceBufferType_Int3   |   4   | 		3	  |   12  |
| LinceBufferType_Int4   |   5   | 		4	  |   16  |
| LinceBufferType_Float  |   6   | 		1	  |   4   |
| LinceBufferType_Float2 |   7   | 		2	  |   8   |
| LinceBufferType_Float3 |   8   | 		3	  |   12  |
| LinceBufferType_Float4 |   9   | 		4	  |   16  |
| LinceBufferType_Mat3   |   10  | 		9	  |   36  |
| LinceBufferType_Mat4   |   11  | 		16	  |   64  |
| LinceBufferType_Count  |   12  | 		--	  |   --  |
*/
typedef enum LinceBufferType {
    LinceBufferType_None = 0, ///< No defined type. Size zero.
    LinceBufferType_Bool,   ///< boolean - identical to int

    LinceBufferType_Int,    ///< int
    LinceBufferType_Int2,   ///< array of 2 ints
    LinceBufferType_Int3,   ///< array of 3 ints
    LinceBufferType_Int4,   ///< array of 4 ints

    LinceBufferType_Float,  ///< float
    LinceBufferType_Float2, ///< array of 2 floats, vec2
    LinceBufferType_Float3, ///< array of 3 floats, vec3
    LinceBufferType_Float4, ///< array of 4 floats, vec4

    LinceBufferType_Mat3,   ///< 3x3 matrices of floats, mat3
    LinceBufferType_Mat4,   ///< 4x4 matrices of floats, mat4
    
    LinceBufferType_Count   ///< number of defined buffer data types
} LinceBufferType;

/** @struct LinceBufferElement
* @brief Describes one element of a buffer layout
*/
typedef struct LinceBufferElement {
    LinceBufferType type;            /**< Lince API data type */
    const char name[LINCE_NAME_MAX]; /**< Name of the parameter in the shader */

    uint32_t gl_type;   /**< Data type defined with OpenGL codes */
    uint32_t comps;     /**< Component count, e.g. Int4 has 4 components */
    uint32_t bytes;     /**< Size in bytes */
    uint32_t offset;    /**< Bytes from the front of vertex layout */
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

/** @struct LinceIndexBuffer
* @brief Stores the order in which vertices are drawn to the screen.
* This is helpful because OpenGL only renders triangles to screen,
* so you have to break-up more complex shapes into triangles.
*/
typedef struct LinceIndexBuffer {
    uint32_t id;     ///< OpenGL renderer ID
    uint32_t count;  ///< number of indices
} LinceIndexBuffer;

/** @brief Create an index buffer with a given set of vertex indices */
LinceIndexBuffer LinceCreateIndexBuffer(uint32_t* data, uint32_t count);

/** @brief Binds an index buffer for rendering */
void LinceBindIndexBuffer(LinceIndexBuffer ib);

/** @brief Unbinds any index buffer currently bound (if any) */
void LinceUnbindIndexBuffer();

/** @brief Removes an index buffer from memory */
void LinceDeleteIndexBuffer(LinceIndexBuffer ib);


#endif // LINCE_BUFFER_H