#include "renderer/vertex_array.h"
#include <glad/glad.h>
#include <stdlib.h>

/* Allocates new vertex array and generates OpenGL ID for it */
LinceVertexArray* LinceCreateVertexArray(LinceIndexBuffer ib){
	LINCE_INFO("Creating vertex array");
	LINCE_INFO("%d bytes\n", (int)sizeof(LinceVertexArray));
	
	LinceVertexArray* va = calloc(1, sizeof(LinceVertexArray));
	LINCE_ASSERT(va, "Failed to allocate memory");

	va->index_buffer = ib;
	va->vb_count = 0;
	va->vb_list = NULL;
	glGenVertexArrays(1, &va->id);
	glBindVertexArray(va->id);
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
	LinceBindIndexBuffer(va->index_buffer);

	// Calculate layout offsets & stride
	unsigned int i, stride = 0;
	for(i =0; i != layout_elements; ++i){
		LinceSetupBufferElementData(&layout[i]);
		layout[i].offset = stride;
		LINCE_INFO(" offset = %u", layout[i].offset);
		stride += layout[i].bytes;
	}
	LINCE_INFO("stride = %u", stride);

	// Set vertex attributes
	for(i = 0; i != layout_elements; ++i){
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i,
			layout[i].comps, // number of components
			layout[i].gl_type, // OpenGL type
			GL_FALSE, // element.norm ? GL_TRUE : GL_FALSE,
			stride,
			(const void*)(const uintptr_t)(layout[i].offset)
		);
	}

	// Append vertex buffer to list
	va->vb_list = realloc(va->vb_list, va->vb_count + 1);
	LINCE_ASSERT(va->vb_list, "Failed to allocate memory");
	va->vb_list[va->vb_count] = vb;
	va->vb_count++;
}

void LinceDeleteVertexArray(LinceVertexArray* va){
	if (!va) return;
	if (va->vb_list && va->vb_count > 0){
		for(int i=0; i!=(int)va->vb_count; ++i){
			LinceDeleteVertexBuffer(va->vb_list[i]);
		}
		free(va->vb_list);
	}
	LinceDeleteIndexBuffer(va->index_buffer);
	glDeleteVertexArrays(1, &va->id);
	free(va);
}