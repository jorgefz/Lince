#ifndef LINCE_RENDERER_H
#define LINCE_RENDERER_H

#include "lince/renderer/buffer.h"
#include "lince/renderer/vertex_array.h"
#include "lince/renderer/shader.h"
#include "lince/renderer/texture.h"
#include "lince/renderer/camera.h"
#include "lince/core/window.h"
#include "lince/tiles/tileset.h"

/* Quad properties, serves as argument for LinceRenderQuad */
typedef struct LinceQuadProps{
	float x, y; 			// 2D position
	float w, h; 			// width, height
	float zorder; 			// order of rendering
	float rotation; 		// clockwise rotation in degrees
	float color[4]; 		// flat quad color - rgba
	LinceTexture* texture;
	LinceTile* tile;
} LinceQuadProps;

/* Initialises renderer state and openGL rendering settings */
void LinceInitRenderer();

/* Terminates renderer state and frees allocated memory */
void LinceTerminateRenderer();

/* Begins rendering scene */
void LinceBeginScene(LinceCamera* cam);

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


#endif // LINCE_RENDERER_H