#include "renderer/tileset.h"

LinceTile LinceGetTile(LinceTexture* texture, vec2 xy, vec2 cellsize, vec2 tilesize){
	
	const float texw = (float)texture->width;
	const float texh = (float)texture->height;

	vec2 min = {
		xy[0] * cellsize[0] / texw,
		xy[1] * cellsize[1] / texh
	};
	vec2 max = {
		(xy[0] + tilesize[0]) * cellsize[0] / texw,
		(xy[1] + tilesize[1]) * cellsize[1] / texh
	};

	LinceTile tile = {
		.texture  = texture,
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

	return tile;
}