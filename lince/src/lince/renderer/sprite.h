#ifndef LINCE_SPRITE_H
#define LINCE_SPRITE_H

#include "lince/renderer/texture.h"
#include "lince/tiles/tileset.h"
#include "lince/renderer/transform.h"
#include "lince/renderer/shader.h"

typedef enum LinceSpriteFlags {
	LinceSprite_UseAlpha = 0x1,
	LinceSprite_UseUV    = 0x2
} LinceSpriteFlags;

/** @struct LinceSprite
* @brief Visual and spatial properties of a rectangle.
*/
typedef struct LinceSprite{
	float x, y;
	float w, h;

	float zorder; 			///< Depth, order of rendering
	float rotation; 		///< Clockwise rotation in degrees
	float color[4]; 		///< Flat color in RGB format
	// float alpha;            ///< Transparency, only used if flag is set
	// float uv[8];			///< Texture coordinates, only used if flag is set
	LinceTexture* texture;	///< Texture to apply. If NULL, only color is used.
	LinceTile* tile;        ///< Texture coordinates
	// LinceSpriteFlags flags; ///< Config flags
} LinceSprite;

// void LinceDrawSprite(LinceSprite* sprite, LinceTransform* transform, LinceShader* shader);

void LinceDrawSprite(LinceSprite* sprite, LinceShader* shader);


#endif /* LINCE_SPRITE_H */