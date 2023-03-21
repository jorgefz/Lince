#ifndef LINCE_TILEMAP_H
#define LINCE_TILEMAP_H

#include "lince/tiles/tileset.h"
#include "lince/renderer/shader.h"
#include "cglm/vec2.h"

/* NOT FOR DOXYGEN
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


/** @struct LinceTilemap
* @brief Holds data for a grid of sprites whose textures
* are picked from a common tileset.
* @todo If the same tileset is used in many other places,
* allow user to specify tile array instead of texture.                     
* @todo Add '-1' index for an 'empty' tile for which to generate no sprite,
* useful for sparse tilemaps e.g. that contain some trees or overlays
*/
typedef struct LinceTilemap{
    LinceTexture* texture; ///< Origin texture from which to pick tiles.
    vec2 cellsize; ///< Size in pixels of a tile in the texture
    array_t tiles; ///< array<LinceTile>: each tile contains texture coordinates.
    array_t sprites; ///< array<LinceSprite>: sprites to render generated from tiles

    vec2 offset;  ///< position offset from world origin. By default, tilemap lower left corner is the origin
    vec2 scale;   ///< scale of individual tiles - default is (1,1)
    float zorder; ///< Depth value at which to draw the tilemap

    uint32_t width;  ///< Width of the map in tiles
    uint32_t height; ///< Height of the map in tiles
    uint32_t* grid;  ///< Indices for which tile to draw at each map location

} LinceTilemap;

/** @brief Initialise tilemap using settings and data pre-defined by the user,
* and provided via the passed handle.
*/
void LinceInitTilemap(LinceTilemap* map);

/** @brief Delete memory allocated within the object, but not the object itself */
void LinceUninitTilemap(LinceTilemap* tm);

/** @brief Simple helper function that iterates through tilemap->sprites
* calling `LinceDrawTilemap` on each.
* @param tm tilemap to draw
* @param shader Shader to use when rendering
*/
void LinceDrawTilemap(LinceTilemap* tm, LinceShader* shader);

#endif /* LINCE_TILEMAP_H */