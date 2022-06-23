#ifndef LINCE_RENDERER_H
#define LINCE_RENDERER_H

#include "renderer/buffer.h"
#include "renderer/vertex_array.h"
#include "renderer/shader.h"
#include "renderer/texture.h"

/* Quad properties, serves as argument for LinceRenderQuad */
typedef struct LinceQuadProps{
	float x, y; // 2D position
	float w, h; // width, height
	float zorder; // order of rendering
	LinceTexture* texture;
	float color[4]; // rgba
} LinceQuadProps;

typedef struct RendererState {
	int dummy;
	// batch pointers, etc
} RendererState;

/* Initialises renderer state and openGL rendering settings */
void LinceInitRenderer();

/* Begins rendering scene */
void LinceBeginScene();

/* Renders scene and flushes batch buffers */
void LinceEndScene();

/* Submits a quad (rectangle) for rendering */
void LinceSubmitQuad();

/* Draws provided vertices directly */
void LinceDrawIndexed(
	LinceShader* shader,
	LinceVertexArray* va,
	LinceIndexBuffer vb
);

/* Empties screen buffer */
void LinceClear();

/* Sets the default screen color */
void LinceSetClearColor(float r, float g, float b, float a);

/*
Generates vertices and adds the quad to the render batch
Example: LinceRenderQuad({ .x=1.0, .y=2.0, .w=1.0, .h=3.0, .texture=tex })
*/
void LinceRenderQuad(LinceQuadProps props);

/* Renders the vertex batch */
void LinceRenderBatch();


#endif // LINCE_RENDERER_H