/** @file renderer.h
* In order to draw stuff to the screen,
* enclose your draw calls between `LinceBeginScene` and `LinceEndScene`,
* and submit sprites with `LinceDrawSprite`.
*/

#ifndef LINCE_RENDERER_H
#define LINCE_RENDERER_H

#include "lince/renderer/buffer.h"
#include "lince/renderer/vertex_array.h"
#include "lince/renderer/shader.h"
#include "lince/renderer/texture.h"
#include "lince/renderer/camera.h"
#include "lince/core/window.h"
#include "lince/renderer/sprite.h"

/// Max number of textures the GPU can bind simultaneously
#define LINCE_MAX_TEXTURE_UNITS 32 


/** @brief Stores information of one vertex */
typedef struct LinceQuadVertex {
	float x, y, z; 	   ///< Position
	float s, t; 	   ///< Texture coordinates
	float color[4];	   ///< RGBA color
	float texture_id;  ///< Binding unit for the texture
} LinceQuadVertex;

typedef struct LinceQuadData {
	float zorder;	///< Depth value
	float rotation;	///< Rotation in degrees
	float color[4];	///< RGBA color
	float uv[8];	///< Texture coordinates
} LinceQuadData;

/** @brief Stores the state of the OpenGL renderer */
typedef struct LinceRenderState {
	LinceShader *default_shader; ///< Default shader to use if none is provided
	LinceShader *shader;		 ///< Shader to use when rendering
	LinceTexture* white_texture; ///< Default texture to use when no texture is used
	
	LinceVertexArray* va; ///< Vertex array of batch
    LinceVertexBuffer vb; ///< Vertex buffer of batch
    LinceIndexBuffer ib;  ///< Index buffer of batch

	unsigned int quad_count;       ///< Number of quads in the batch
	LinceQuadVertex* vertex_batch; ///< Quads to render
	unsigned int* index_batch;     ///< Indices of batch

	unsigned int texture_slot_count;	///< Number of active texture units
	LinceTexture* texture_slots[LINCE_MAX_TEXTURE_UNITS]; ///< Active textures

} LinceRenderState;

/** @brief Returns the global renderer state */
LinceRenderState* LinceGetRenderer();

/** @brief Calculates the z-order based on the 'y' value of the position,
* such that objects at lower 'y' are drawn objects at higher 'y'.
* @param y y-coordinate
* @param ylim limits (min,max) on the value of y
* @param zlim limits (min,max) on the value of the depth
* @returns the z depth value 
*
* Equation for normalising a number in a range `a` to another range `b`:
*   `xnorm = (bmax - bmin) * (x - amin) / (amax - amin) + bmin`
*/
float LinceYSortedZ(float y, vec2 ylim, vec2 zlim);

/** @brief Initialises renderer state and openGL rendering settings */
void LinceInitRenderer();

/** @brief Terminates renderer state and frees allocated memory */
void LinceTerminateRenderer();

/** @brief Begins a rendering scene and initialsies batch buffers
* @param cam Camera required for the view-projection matrix
*/
void LinceBeginRender(LinceCamera* cam);

/** @brief Renders scene and flushes batch buffers to the screen */
void LinceEndRender();

/** @brief Draw stored vertices and clear vertex batch.
 * Useful to draw sprites using same shader with different values for uniforms.
*/
void LinceFlushRender();

/** @brief Submits a quad for rendering 
 * @param data Graphical properties of the quad
 * @param transform Size and location
 * @param texture Texture to apply to the quad. If NULL, a white texture is used.
 * @param shader Shader to apply. If NULL, a default shader is used.
*/
void LinceDrawQuad(
	LinceTransform *transform,
	LinceQuadData  *data,
	LinceTexture   *texture,
	LinceShader    *shader
);

/** @brief Draws provided vertices directly */
void LinceDrawIndexed(
	LinceShader* shader,
	LinceVertexArray* va,
	LinceIndexBuffer vb
);

/** @brief Empties screen buffer */
void LinceClear();

/** @brief Sets the default background screen color */
void LinceSetClearColor(float r, float g, float b, float a);



#endif // LINCE_RENDERER_H