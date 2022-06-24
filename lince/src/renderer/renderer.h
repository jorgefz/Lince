#ifndef LINCE_RENDERER_H
#define LINCE_RENDERER_H

#include "renderer/buffer.h"
#include "renderer/vertex_array.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "core/window.h"

// TEMPORARY
#include "renderer/camera.h"
typedef struct LinceRendererState {
	LinceWindow* window;
	LinceCamera* cam;
	LinceShader* shader;
	LinceTexture* white_texture;
	
	LinceVertexArray* va;
    LinceVertexBuffer vb;
    LinceIndexBuffer ib;

} LinceRendererState;

LinceRendererState* LinceGetRendererState();


/* Quad properties, serves as argument for LinceRenderQuad */
typedef struct LinceQuadProps{
	float x, y; // 2D position
	float w, h; // width, height
	float zorder; // order of rendering
	LinceTexture* texture;
	float color[4]; // rgba
} LinceQuadProps;

/* Initialises renderer state and openGL rendering settings */
void LinceInitRenderer(LinceWindow* window);

/* Terminates renderer state and frees allocated memory */
void LinceTerminateRenderer();

/* Begins rendering scene */
void LinceBeginScene();

/* Renders scene and flushes batch buffers */
void LinceEndScene();

/* Submits a quad for rendering
e.g LinceSubmitQuad({.x=1.0, .y=2.0});
C99 standard guarantees that uninitialised members
are set to zero if at least one has been initialised
within the brace-enclosed list.
*/
void LinceDrawQuad(LinceQuadProps props);



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


/* Renders the vertex batch */
void LinceRenderBatch();


#endif // LINCE_RENDERER_H