#include "core/profiler.h"
#include "core/memory.h"
#include "renderer/vertex_array.h"
#include <glad/glad.h>
#include <stdlib.h>

/* Allocates new vertex array and generates OpenGL ID for it */
LinceVertexArray* LinceCreateVertexArray(LinceIndexBuffer ib){
	LINCE_PROFILER_START(timer);
	LINCE_INFO("Creating Vertex Array");

	LinceVertexArray* va = LinceCalloc(sizeof(LinceVertexArray));

	va->index_buffer = ib;
	va->vb_count = 0;
	va->vb_list = NULL;
	glGenVertexArrays(1, &va->id);
	glBindVertexArray(va->id);
	LINCE_PROFILER_END(timer);
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
	uint32_t layout_elements
){
	LINCE_PROFILER_START(timer);

	LinceBindVertexArray(va);
	LinceBindVertexBuffer(vb);
	LinceBindIndexBuffer(va->index_buffer);

	// Calculate layout offsets & stride
	uint32_t i, stride = 0;
	for(i =0; i != layout_elements; ++i){
		LinceSetupBufferElementData(&layout[i]);
		layout[i].offset = stride;
		stride += layout[i].bytes;
	}

	// Logging
	LINCE_INFO("Adding Vertex Array Attributes...");
	LINCE_INFO(" --> Stride: %d bytes", (int)stride);
	LINCE_INFO(" --> Attribute : Size  Ncomp Offset");
	LINCE_INFO(" --> ----------  ----  ----- ------");
	for(i=0; i != layout_elements; ++i){
		LINCE_INFO(" --> %-10s: %-5d %-5d %-5d",
			layout[i].name,
			(int)layout[i].bytes,
			(int)layout[i].comps,
			(int)layout[i].offset
		);
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
			(const void*)(const uintptr_t)(layout[i].offset)
		);
	}

	// Append vertex buffer to list
	va->vb_list = LinceRealloc(va->vb_list, (va->vb_count + 1)*sizeof(LinceVertexBuffer));
	LINCE_ASSERT_ALLOC(va->vb_list, (va->vb_count + 1)*sizeof(LinceVertexBuffer));
	va->vb_list[va->vb_count] = vb;
	va->vb_count++;

	LINCE_PROFILER_END(timer);
}

void LinceDeleteVertexArray(LinceVertexArray* va){
	LINCE_INFO("Deleting Vertex Array");
	if (!va) return;
	if (va->vb_list && va->vb_count > 0){
		for(int i=0; i!=(int)va->vb_count; ++i){
			LinceDeleteVertexBuffer(va->vb_list[i]);
		}
		LinceFree(va->vb_list);
	}
	LinceDeleteIndexBuffer(va->index_buffer);
	glDeleteVertexArrays(1, &va->id);
	LinceFree(va);
}