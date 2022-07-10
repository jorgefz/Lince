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
*/
LinceTile LinceGetTile(LinceTexture* texture, vec2 pos, vec2 cellsize, vec2 tilesize);


#endif /* LINCE_TILESET_H */