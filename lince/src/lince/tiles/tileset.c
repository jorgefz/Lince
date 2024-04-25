#include "tiles/tileset.h"

/** @brief Calculates the texture coordinates of all cells in a tileset.
 * @param tileset Tileset
*/
static void LinceTilesetCalculateCoords(LinceTileset* tileset){
	float cellw = tileset->cellsize.x / tileset->texsize.x;
	float cellh = tileset->cellsize.y / tileset->texsize.y;
	uint32_t xcells = tileset->xcells;
	uint32_t ycells = tileset->ycells;
	
	array_init(&tileset->coords, sizeof(LinceRect));
	array_resize(&tileset->coords, xcells*ycells);

	for (uint32_t x = 0; x != xcells; ++x){
		for (uint32_t y = 0; y != ycells; ++y){
			LinceRect tile = {.x=(float)x*cellw, .y=(float)y*cellh, .w=cellw, .h=cellh};
			array_set(&tileset->coords, &tile, xcells * y + x);
		}
	}
}


LinceTileset* LinceTilesetInit(LinceTileset* tileset, LinceTexture* tex, uint32_t cell_width, uint32_t cell_height){
	tileset->texture = tex;
	tileset->texsize = (LincePoint){.x = (float)tex->width, .y = (float)tex->height};
	tileset->cellsize = (LincePoint){.x = (float)cell_width, .y = (float)cell_height};
	tileset->xcells = tex->width / cell_width;
	tileset->ycells = tex->height / cell_height;
	LinceTilesetCalculateCoords(tileset);
	return tileset;
}

void LinceTilesetUninit(LinceTileset* tset){
	array_uninit(&tset->coords);
}

LinceRect* LinceTilesetGetTileCoords(LinceTileset* tileset, uint32_t x, uint32_t y){
	return array_get(&tileset->coords, tileset->xcells * y + x);
}

