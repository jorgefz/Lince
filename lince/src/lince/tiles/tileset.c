#include "tiles/tileset.h"

void LinceGetTileCoords(
	LinceTile* tile,
	vec2 texsize,
	vec2 xy,
	vec2 cellsize,
	vec2 tilesize
) {
	const float texw = texsize[0], texh = texsize[1];
	vec2 min = { xy[0]*cellsize[0]/texw, xy[1]*cellsize[1]/texh};
	vec2 max = {
		(xy[0]+tilesize[0])*cellsize[0]/texw,
		(xy[1]+tilesize[1])*cellsize[1]/texh
	};

	*tile = (LinceTile){
		.texsize  = {texw, texh},
		.pos = {xy[0], xy[1]},
		.cellsize = {cellsize[0], cellsize[1]},
		.tilesize = {tilesize[0], tilesize[1]},
		.coords = {
			min[0], min[1],
			max[0], min[1],
			max[0], max[1],
			min[0], max[1]
		}
	};
}


LinceTexture* LinceLoadTextureWithTiles(
	const char* fname,	// Texture filename
	array_t* tiles,		// array<LinceTile>, returns collected tiles. Must be uninitialised.
	vec2 cellsize		// Size of a tile/cell in pixels 
) {
	LinceTexture* tex = LinceCreateTexture(fname, fname);
	vec2 texsize = {(float)tex->width, (float)tex->height};
	vec2 tilesize = {1,1};
	uint32_t xtiles = tex->width/(uint32_t)cellsize[0];
    uint32_t ytiles = tex->height/(uint32_t)cellsize[1];
	array_init(tiles, sizeof(LinceTile));

	/* Adds tiles from left to right, and then top to bottom */
	for(size_t y = 0; y != ytiles; ++y){
        for(size_t x = 0; x != xtiles; ++x){
			LinceTile tile;
			vec2 pos = {(float)x, (float)(ytiles-y+1)};
            LinceGetTileCoords(&tile, texsize, pos, cellsize, tilesize);
			array_push_back(tiles, &tile);
        }
    }

	return tex;
}
