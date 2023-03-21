#ifndef LINCE_VERTEX_ARRAY_H
#define LINCE_VERTEX_ARRAY_H

#include "lince/renderer/buffer.h"

/** @struct LinceVertexArray
* @brief Holds the information for a bacth of vertices to be drawn to the screen.
*/
typedef struct LinceVertexArray {
	uint32_t id; 					///< OpenGL renderer ID
	LinceIndexBuffer index_buffer;	///< Order in whcih vertices are drawn.
	LinceVertexBuffer* vb_list; 	///< Data buffer with vertices
	uint32_t vb_count; 				///< Number of vertices
} LinceVertexArray;

/** @brief Allocates new vertex array and generates an OpenGL ID for it */
LinceVertexArray* LinceCreateVertexArray(LinceIndexBuffer index_buffer);

/** @brief Binds a vertex array for rendering */
void LinceBindVertexArray(LinceVertexArray* vertex_array);

/** @brief Unbinds the active vertex array (if any) */
void LinceUnbindVertexArray(void);

/** @brief Sets up vertex buffer attributes and appends given buffer */
void LinceAddVertexArrayAttributes(
	LinceVertexArray* vertex_array,  ///< Vertex array to set up
	LinceVertexBuffer vertex_buffer, ///< Buffer of vertices to append
	LinceBufferElement* layout,      ///< Layout of buffer elements,
								     ///< tells how tin interpret vertex buffer data
	uint32_t layout_elements 		 ///< Number of buffer elements in layout
);

/** @brief Deallocates and deletes vertex array */
void LinceDeleteVertexArray(LinceVertexArray* vertex_array);


#endif // LINCE_VERTEX_ARRAY_H