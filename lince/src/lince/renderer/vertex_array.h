#ifndef LINCE_VERTEX_ARRAY_H
#define LINCE_VERTEX_ARRAY_H

#include "lince/renderer/buffer.h"

typedef struct LinceVertexArray {
	unsigned int id; // OpenGL renderer ID
	LinceIndexBuffer index_buffer;
	LinceVertexBuffer* vb_list; /* associated vertex buffers */
	unsigned int vb_count; /* number of vertex buffers */
} LinceVertexArray;

/* Allocates new vertex array and generates an OpenGL ID for it */
LinceVertexArray* LinceCreateVertexArray(LinceIndexBuffer index_buffer);

void LinceBindVertexArray(LinceVertexArray* vertex_array);

void LinceUnbindVertexArray(void);

/* Sets up vertex buffer attributes and appends given buffer */
void LinceAddVertexArrayAttributes(
	LinceVertexArray* vertex_array,  /* must have been initialised already */
	LinceVertexBuffer vertex_buffer, /* stores raw vertex data */
	LinceBufferElement* layout,      /* list of buffer elements that 
								        together constitute the data layout */
	unsigned int layout_elements     /* Number of buffer elements */
);

void LinceDeleteVertexArray(LinceVertexArray* vertex_array);


#endif // LINCE_VERTEX_ARRAY_H