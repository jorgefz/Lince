
#include <stdarg.h>
#include "renderer/buffer.h"
#include "core/core.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

/* --- Buffer Element & Layout ---*/

/* Data on each buffer data type */
static const LinceBufferElement buffer_types_data[] = {
    {.type=LinceBufferType_None,   .gl_type=GL_NONE,  .comps=0, .bytes=0},
    {.type=LinceBufferType_Bool,   .gl_type=GL_BOOL,  .comps=1, .bytes=sizeof(int)  },
    {.type=LinceBufferType_Int,    .gl_type=GL_INT,   .comps=1, .bytes=sizeof(int)  },
	{.type=LinceBufferType_Int2,   .gl_type=GL_INT,   .comps=2, .bytes=sizeof(int)*2},
	{.type=LinceBufferType_Int3,   .gl_type=GL_INT,   .comps=3, .bytes=sizeof(int)*3},
	{.type=LinceBufferType_Int4,   .gl_type=GL_INT,   .comps=4, .bytes=sizeof(int)*4},

    {.type=LinceBufferType_Float,  .gl_type=GL_FLOAT, .comps=1, .bytes=sizeof(float)  },
	{.type=LinceBufferType_Float2, .gl_type=GL_FLOAT, .comps=2, .bytes=sizeof(float)*2},
	{.type=LinceBufferType_Float3, .gl_type=GL_FLOAT, .comps=3, .bytes=sizeof(float)*3},
	{.type=LinceBufferType_Float4, .gl_type=GL_FLOAT, .comps=4, .bytes=sizeof(float)*4},

	{.type=LinceBufferType_Mat3,   .gl_type=GL_FLOAT, .comps=3*3, .bytes=sizeof(float)*3*3},
	{.type=LinceBufferType_Mat4,   .gl_type=GL_FLOAT, .comps=4*4, .bytes=sizeof(float)*4*4},
};

/* Provides the unique data of a buffer type: component count, size, and OpenGL type */
static LinceBufferElement LinceGetBufferTypeData(LinceBufferType type){
	LINCE_ASSERT(type < LinceBufferType_Count, "Unknown buffer data type");
	return buffer_types_data[type];
}

static void DebugPrint_BufferLayout(LinceBufferElement* layout, int len){
	for(int i=0; i!=len; ++i){
		printf("%d) '%s': type %d, %d comps and %d bytes",
			i, layout[i].name, layout[i].type,
			layout[i].comps, layout[i].bytes
		);
	}
}


/* --- Vertex Buffer --- */

LinceVertexBuffer CreateVertexBuffer(float* data, unsigned int size){
	unsigned int id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	int draw_mode = data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
	glBufferData(GL_ARRAY_BUFFER, size, data, draw_mode);
	return (LinceVertexBuffer)id;
}

void LinceSetVertexBufferData(LinceVertexBuffer vb, float* data, unsigned int size){
	LinceBindVertexBuffer(vb);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void LinceBindVertexBuffer(LinceVertexBuffer vb){
	glBindBuffer(GL_ARRAY_BUFFER, vb);
}

void LinceUnbindVertexBuffer(){
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LinceDeleteVertexBuffer(LinceVertexBuffer vb){
	glDeleteBuffers(1, &vb);
}


/* --- Index Buffer --- */

LinceIndexBuffer LinceCreateIndexBuffer(unsigned int* data, unsigned int count){
	LinceIndexBuffer ib = {.id=0, .count=count};
	glGenBuffers(1, &ib.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
	return ib;
}

void LinceBindIndexBuffer(LinceIndexBuffer ib){
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.id);
}

void LinceUnbindIndexBuffer(){
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void LinceDeleteIndexBuffer(LinceIndexBuffer ib){
	glDeleteBuffers(1, &ib.id);
}