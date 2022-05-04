#ifndef LINCE_RENDERER_H
#define LINCE_RENDERER_H

#include "renderer/buffer.h"
#include "renderer/vertex_array.h"
#include "renderer/shader.h"

void LinceRender_Init();
void LinceRender_BeginScene();
void LinceRender_EndScene();
void LinceRender_Submit();

void LinceClear();
void LinceSetClearColor(float r, float g, float b, float a);
void LinceDrawIndexed(
	LinceShader* shader, LinceVertexArray* va, LinceIndexBuffer vb);


// Future rendering API

/* Quad properties, serves as argument for LinceRenderQuad */
typedef struct LinceQuadProps{
	double x, y; // 2D position
	double w, h; // width, height
	double zorder; // order of rendering
	void *texture;
	float color[4]; // rgba
} LinceQuadProps;

typedef struct RendererState {
	int dummy;
	// batch pointers, etc
} RendererState;

/*
Generates vertices and adds the quad to the render batch
Example: LinceRenderQuad({ .x=1.0, .y=2.0, .w=1.0, .h=3.0, .texture=tex })
*/
void LinceRenderQuad(LinceQuadProps props);

/*
Renders the vertex batch
*/
void LinceRenderBatch();


#endif // LINCE_RENDERER_H