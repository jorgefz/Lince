#ifndef LINCE_TILESET_H
#define LINCE_TILESET_H

#include "lince/containers/array.h"
#include "lince/renderer/texture.h"
#include "cglm/vec2.h"

/** @struct LinceTile
* @brief Stores information about a tile, which is a region within a texture
* defined via texture coordinates.
* @todo Only `float coords[8]` is strictly necessary
*/
typedef struct LinceTile {
	vec2 texsize;		///< Size of the parent texture in pixels
	vec2 pos;			///< Location of the tile in units of cells
	vec2 cellsize;		///< Size of a cell in pixels
	vec2 tilesize;   	///< Size of tile in units of cells
	float coords[8]; 	///< Texture coordinates
} LinceTile;

/** @brief Generates texture coordinates for a region in a texture.
* 
* @note Parameters may specify a tile outside texture bounds.
* 
* Code example:
* ```c
* LinceTexture* tileset;
* vec2 texsize = {(float)tileset->height, (float)tileset->width};
* LinceTile grass;
* LinceGetTile(
* 	&grass		// Return tile
* 	texsize, 	// size of original texture
* 	(vec2){1, 2},	// location of the tile in cells within the texture
* 	(vec2){16,16},	// size of pixels of a cell, e.g. 16x16 pixels
* 	(vec2){1,1}		// size of a tile in cells, e.g. 1x1 cells
* 					// use larger sizes for tiles that span mutiple cells,
* 					// e.g. houses or trees
* )
* ```
*/
void LinceGetTileCoords(
	LinceTile* tile,	///< Return value. Tile coordinates are written here.
	vec2 texsize,		///< Size of the parent texture in pixels.
	vec2 pos,			///< Position of the tile in cells from lower left corner.
	vec2 cellsize,		///< Size of a cell in pixels.
	vec2 tilesize		///< Size of the tile in cells.
);

/** @brief Calculates all tile coordinates in a texture.
* 
* The tiles are copied over to the array argument,
* which must point to valid memory but not be initialised.
* The tiles are loaded from the texture from left to right, and bottom to top.
* @todo Change tile loading to 'top to bottom'.
*/
void LinceGetTilesFromTexture(
	LinceTexture* texture,	///< Texture filename
	vec2 cellsize,			///< Size of a cell in pixels 
	array_t* tiles			///< array<LinceTile>, returns collected tiles.
);

/** @brief Loads a texture and collects all tiles within it.
* 
* The texture is returned, but the tiles are copied over to
* the array argument, which must not be initialised.
* The tiles are loaded from the texture from left to right, and bottom to top.
*/
LinceTexture* LinceLoadTextureWithTiles(
	const char* fname,	///< Texture filename
	vec2 cellsize,		///< Size of a cell in pixels 
	array_t* tiles		///< array<LinceTile>, returns collected tiles.
);


#endif /* LINCE_TILESET_H */