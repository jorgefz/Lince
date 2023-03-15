#ifndef LINCE_TILEMAP_H
#define LINCE_TILEMAP_H

#include "lince/tiles/tileset.h"
#include "lince/renderer/shader.h"
#include "cglm/vec2.h"

/*
Simplest tilemap:
- array of sprites

Generate from input parameters:
- base texture
- tileset cellsize (tilesize assumed [1,1] )
- width and height in tiles
- array of indices of size width*height,
    corresponds to indices of tiles in teleset
- grid offset, scale, and z-value

(1) Generate LinceTiles from texture tileset
(2) Ensure all input indices are valid
(3) Iterate each tile index, append sprite with given offset, size, LinceTile, and zorder
Pros: no need for custom draw function. User may just draw sprite list in a loop.

-- Init function should
    a) Pass argument list and return allocated tilemap
        Pros: Lightweight tilemap struct, only array of sprites and mapsize
        Cons: Long argument list. Memory is not user managed.
    b) Pass tilemap handle, which is modified with data
        Pros: Easier to manage long argument list
        Cons: Large tilemap struct
    c) Two structs: settings and tilegrid
        Pros: bets of (a) and (b)
        Cons: two structs??

CreateTilemap(mapsize, map_indices, texture, cellsize, offset, scale, zorder)

struct Tilemap {
    // input
    vec2 mapsize;
    array_t<uint32_t> tile_indices;
    LinceTexture* tileset;
    vec2 tile_cellsize;
    vec2 offset, scale;
    float zorder;

    // output
    array_t<LinceSprite> sprites;
}

*/

typedef struct LinceTilemap{
    LinceTexture* texture; // texture tileset
    vec2 cellsize;   // size in pixels of a tile in the texture
    array_t tiles;   // array<LinceTile>: necessary for sprites
    array_t sprites; // array<LinceSprite>: tile sprites to render

    vec2 offset; // position offset from world origin
    vec2 scale;  // scale of individual tiles - default is (1,1)
    float zorder;

    uint32_t width, height; // dimensions of the map in tiles
    uint32_t* grid;         // indices for tiles

    // CallbackFn on_teleport(this, target-tilemap, trigger, ...)
} LinceTilemap;

void LinceInitTilemap(LinceTilemap* map);

void LinceUninitTilemap(LinceTilemap* tm);

void LinceDrawTilemap(LinceTilemap* tm, LinceShader* shader);

#endif /* LINCE_TILEMAP_H */