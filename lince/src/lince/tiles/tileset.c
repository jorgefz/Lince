#include "tiles/tileset.h"

void LinceGetTileCoords(LinceTile* tile, vec2 texsize, vec2 xy, vec2 cellsize, vec2 tilesize){
	
	const float texw = texsize[0], texh = texsize[1];

	vec2 min = {
		xy[0] * cellsize[0] / texw,
		xy[1] * cellsize[1] / texh
	};
	vec2 max = {
		(xy[0] + tilesize[0]) * cellsize[0] / texw,
		(xy[1] + tilesize[1]) * cellsize[1] / texh
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