#ifndef LINCE_TILESET_H
#define LINCE_TILESET_H

#include "renderer/texture.h"
#include "cglm/vec2.h"


typedef struct LinceTile {
	LinceTexture* texture;
	vec2 pos;			// location of the tile in units of cells
	vec2 cellsize;		// size of a cell in pixels
	vec2 tilesize;   	// size of tile in units of cells
	float coords[8]; 	// texture coordinates
} LinceTile;

/*
Generates texture coordinates for a tile within a texture
given the size of a cell (unit tile) in pixels.
Parameters:
	texture:	(LinceTexture*) loaded texture
	pos:		(cglm/vec2) Position of the tile in cells from lower left corner
	cellsize:	(cglm/vec2) Size of a cell within the texture in pixels
	tilesize:	(cglm/vec2) Size of the tile in cells
Returns:
	tile:		(LinceTile) Struct that contains the above parameters
				plus the generated texture coordinates.
Example:
	LinceTile grass = LinceGetTile(
		tileset,		// original texture / tileset
		(vec2){1, 2},	// location of the tile, e.g. x=1 y=2 within the texture
		(vec2){16,16},	// size on pixels of a cell, e.g. 16x16 pixels
		(vec2){1,1}		// size of a tile in cells, e.g. 1x1 cells
						// use larger sizes for tiles that span mutiple cells,
						// e.g. houses or trees
	)
*/
LinceTile LinceGetTile(LinceTexture* texture, vec2 pos, vec2 cellsize, vec2 tilesize);


#endif /* LINCE_TILESET_H */