#include "renderer/vertex_array.h"

/* Allocates new vertex array and generates OpenGL ID for it */
LinceVertexArray* LinceCreateVertexArray(LinceIndexBuffer ib){
	LinceVertexArray* va = calloc(1, sizeof(LinceVertexArray));
	LINCE_ASSERT(va, "Failed to allocate memory");
	va->index_buffer = ib;
	glCreateVertexArrays(1, &va->id);
	return va;
}

void LinceBindVertexArray(LinceVertexArray* va){
	glBindVertexArray(va->id);
}

void LinceUnbindVertexArray(void){
	glBindVertexArray(0);
}

/* Sets up vertex buffer attributes on the vertex array */
void LinceAddVertexArrayAttributes(
	LinceVertexArray* va,
	LinceVertexBuffer vb,
	LinceBufferElement* layout,
	unsigned int layout_elements
){

	LinceBindVertexArray(va);
	LinceBindVertexBuffer(vb);
	LinceBindIndexBuffer(va->ib);

	// Calculate layout offsets & stride
	unsigned int i, stride;
	for(i =0; i != layout_elements; ++i){
		LinceSetupBufferElementData(&layout[i]);
		layout[i].offset = stride;
		stride += layout[i].bytes;
	}

	// Set vertex attributes
	for(i = 0; i != layout_elements; ++i){
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i,
			layout[i].comps, // number of components
			layout[i].gl_type, // OpenGL type
			GL_FALSE, // element.norm ? GL_TRUE : GL_FALSE,
			stride,
			(const void*)(const uintptr_t)layout[i].offset
		);
		index++;
	}

	// Append vertex buffer to list
	va->vb_list = realloc(va->vb_list, va->vb_count + 1);
	LINCE_ASSERT(va->vb_list, "Failed to allocate memory");
	va->vb_list[va->vb_count] = vb;
	va->vb_count++;
}

void LinceDeleteVertexArray(LinceVertexArray* va){
	if (!va) return;
	glDeleteVertexArrays(1, &va->id);
	if (va->vb_list) free(va->vb_list);
	free(va);
}