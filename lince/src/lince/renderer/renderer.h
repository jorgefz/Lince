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
#include "lince/tiles/tileset.h"

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

/** @struct LinceSprite
* @brief Visual and spatial properties of a rectangle.
*/
typedef struct LinceSprite{
	float x, y; 			///< 2D position
	float w, h; 			///< Width and height
	float zorder; 			///< Depth, order of rendering
	float rotation; 		///< Clockwise rotation in degrees
	float color[4]; 		///< Flat color in RGBA format
	LinceTexture* texture;	///< LinceTexture object. If NULL, only colour is used.
	LinceTile* tile;		///< LinceTile or subtexture. If NULL, full texture is used.
} LinceSprite;

/** @brief Initialises renderer state and openGL rendering settings */
void LinceInitRenderer();

/** @brief Terminates renderer state and frees allocated memory */
void LinceTerminateRenderer();

/** @brief Begins a rendering scene and initialsies batch buffers
* @param cam Camera required for the view-projection matrix
*/
void LinceBeginScene(LinceCamera* cam);

/** @brief Renders scene and flushes batch buffers to the screen */
void LinceEndScene();

/** @brief Submits a recangle sprite for rendering
* @param sprite Sprite to render
* @param shader LinceShader to bind. If NULL, a default minimal shader is used.
*/
void LinceDrawSprite(LinceSprite* sprite, LinceShader* shader);

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

/** @brief Draw stored vertices and clear vertex batch */
void LinceStartNewBatch();


#endif // LINCE_RENDERER_H