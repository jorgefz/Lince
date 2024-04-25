#ifndef LINCE_SPRITE_H
#define LINCE_SPRITE_H

#include "lince/renderer/texture.h"
#include "lince/tiles/tileset.h"
#include "lince/renderer/transform.h"
#include "lince/renderer/shader.h"

/** @brief Defines a RGB color */
typedef struct LinceColor {
	float r, g, b; ///< Red, green, and blue channels
} LinceColor;

typedef enum LinceSpriteFlags {
	LinceSprite_UseAlpha = 0x1 ///< Enable transparency
} LinceSpriteFlags;

/** @struct LinceSprite
* @brief Visual and spatial properties of a rectangle.
*/
typedef struct LinceSprite{
	float zorder; 			///< Depth, order of rendering
	float rotation; 		///< Clockwise rotation in degrees
	LinceColor color; 		///< Flat color in RGB format
	float alpha;            ///< Transparency, only used if flag is set
	LinceTexture* texture;	///< Texture to apply. If NULL, only color is used.
	LinceSpriteFlags flags; ///< Config flags
} LinceSprite;

/** @brief Submits a sprite to the renderer.
 * @param sprite Sprite
 * @param transform Transform (e.g. position and scale)
 * @param shader Shader to apply. If null, default is used.
*/
void LinceDrawSprite(LinceSprite* sprite, LinceTransform* transform, LinceShader* shader);

/** @brief Submits a region of a sprite to the renderer, defined by texture coordinates.
 * @param sprite Sprite
 * @param transform Transform (e.g. position and scale)
 * @param uv Rect defining the bounds of the region within the texture to draw
 * @param shader Shader to apply. If null, default is used.
*/
void LinceDrawSpriteTile(LinceSprite* sprite, LinceTransform* transform, LinceRect* uv, LinceShader* shader);


#endif /* LINCE_SPRITE_H */