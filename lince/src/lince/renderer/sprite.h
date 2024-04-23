#ifndef LINCE_SPRITE_H
#define LINCE_SPRITE_H

#include "lince/renderer/texture.h"
#include "lince/tiles/tileset.h"

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

#endif /* LINCE_SPRITE_H */