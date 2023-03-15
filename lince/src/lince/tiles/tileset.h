#ifndef LINCE_TILESET_H
#define LINCE_TILESET_H

#include "lince/containers/array.h"
#include "lince/renderer/texture.h"
#include "cglm/vec2.h"

// Only `float coords[8]` is strictly necessary
typedef struct LinceTile {
	vec2 texsize;		// Size of the parent texture in pixels
	vec2 pos;			// location of the tile in units of cells
	vec2 cellsize;		// size of a cell in pixels
	vec2 tilesize;   	// size of tile in units of cells
	float coords[8]; 	// texture coordinates
} LinceTile;

/*
Generates texture coordinates for a region in a texture.
Note that parameters may specify a tile outside texture bounds.

Example
-------
	LinceTexture* tileset;
	vec2 texsize = {(float)tileset->height, (float)tileset->width};
	LinceTile grass;
	LinceGetTile(
		&grass		// Return tile
		texsize, 	// size of original texture
		(vec2){1, 2},	// location of the tile in cells within the texture
		(vec2){16,16},	// size of pixels of a cell, e.g. 16x16 pixels
		(vec2){1,1}		// size of a tile in cells, e.g. 1x1 cells
						// use larger sizes for tiles that span mutiple cells,
						// e.g. houses or trees
	)
*/
void LinceGetTileCoords(
	LinceTile* tile,	/* Return value. Tile coordinates are written here. */
	vec2 texsize,		/* Size of the parent texture in pixels. */
	vec2 pos,			/* Position of the tile in cells from lower left corner. */
	vec2 cellsize,		/* Size of a cell in pixels. */
	vec2 tilesize		/* Size of the tile in cells. */
);

/*
Loads a texture and collects all tiles within it.
Returns the texture object, and passes
*/
LinceTexture* LinceLoadTextureWithTiles(
	const char* fname,	// Texture filename
	array_t* tiles,		// array<LinceTile>, returns collected tiles. Must be uninitialised.
	vec2 cellsize		// Size of a cell in pixels 
);


#endif /* LINCE_TILESET_H */