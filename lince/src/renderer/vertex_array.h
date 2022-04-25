#ifndef LINCE_VERTEX_ARRAY_H
#define LINCE_VERTEX_ARRAY_H

#include "renderer/buffer.h"

typedef struct LinceVertexArray {
	unsigned int id;
	LinceIndexBuffer index_buffer;
	LinceVertexBuffer* vb_list;
	unsigned int vb_count;
} LinceVertexArray;

/* Allocates new vertex array and generates OpenGL ID for it */
LinceVertexArray* LinceCreateVertexArray(LinceIndexBuffer ib);

void LinceBindVertexArray(LinceVertexArray* va);

void LinceUnbindVertexArray();

/* Sets up vertex buffer attributes and appends given buffer */
void LinceAddVertexArrayAttributes(
	LinceVertexArray* va, LinceVertexBuffer vb,
	LinceBufferElement* layout, unsigned int layout_elements
);

void LinceDeleteVertexArray(LinceVertexArray* va);


#endif // LINCE_VERTEX_ARRAY_H