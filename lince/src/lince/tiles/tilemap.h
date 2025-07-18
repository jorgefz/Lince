#ifndef LINCE_TILEMAP_H
#define LINCE_TILEMAP_H

#include "lince/tiles/tileset.h"
#include "lince/renderer/shader.h"
#include "lince/renderer/sprite.h"


/** @struct LinceTilemap
 * @brief Grid of sprites that use the same tileset.
* @todo If the same tileset is used in many other places,
* allow user to specify tile array instead of texture.                     
* @todo Add '-1' index for an 'empty' tile for which to generate no sprite,
* useful for sparse tilemaps e.g. that contain some trees or overlays
*/
typedef struct LinceTilemap{
    // User-defined parameters 
    uint32_t width;   ///< Width of the map in tiles
    uint32_t height;  ///< Height of the map in tiles
    LincePoint pos;   ///< Location of the center of the map
    LincePoint scale; ///< Size of each tile in world coordinates
    
    // Internal data
    LinceTileset* tileset; ///< Tileset
    LinceSprite sprite;    ///< Sprite shared by all tiles
    array_t transforms;    ///< array<LinceTransform> Transforms of each map tile
    array_t indices;       ///< array<uint32_t> Indices of tiles in tileset to draw at each location

} LinceTilemap;

/** @brief Initialise a tilemap
 * @param map Output map. Must have width, height, pos, and scale.
 * @param map_grid Indices of the tiles to draw at each map location,
 *                  with origin on the lower left corner.
 * @return Pointer to input map.
*/
LinceTilemap* LinceTilemapInit(LinceTilemap* map, uint32_t* map_grid);

/** @brief Frees memory allocated by tilemap. This does not include the tileset! */
void LinceTilemapUninit(LinceTilemap* map);

/** @brief Use an existing tileset to draw a tilemap.
 * This function must be called at least once before `LinceDrawTilemap`.
 * @param map Tilemap
 * @param tileset Tileset
*/
void LinceTilemapUseTileset(LinceTilemap* map, LinceTileset* tileset);

/** @brief Draw a tilemap to the screen.
 * @param map Tilemap
 * @param shader Shader to apply. If null, default is used.
 * @note Do not draw tiles outside of the screen!
*/
void LinceDrawTilemap(LinceTilemap* map, LinceShader* shader);

/** @brief Load a tilemap from a TOML file.
 * The TOML file must have the following parameters:
 *   mapwidth    :(int) Width of the map in tiles.
 *   mapheight   :(int) Height of the map in tiles.
 *   grid        :(Array[float]) Array of length mapwidth*mapheight storing the indices of the tiles
 *                               to draw at each location.
 *   centerx     :(float, optional) Location of the centre of the map in the X axis. Default is 0.
 *   centery     :(float, optional) Location of the centre of the map in the Y axis. Default is 0.
 *   scalewidth  :(float, optional) Width of each tile in world coordinates. Default is 1.
 *   scaleheight :(float, optional) Height of each tile in world coordinates. Default is 1.
 *   tileset     :(str, optional)   Tileset used to draw the map tiles
 */
void* LinceLoadTilemapAsset(LinceAssetCache* cache, string_t path, void* args);

/** @brief Unload tilemap loaded with `LinceLoadTilemapAsset` */
void LinceUnloadTilemapAsset(LinceAssetCache* cache, void* obj);

/** @brief Write tilemap data to disk.
 * @param cache Asset cache
 * @param asset_sid SID of tilemap asset to write to disk.
 * @param tileset Path to tileset.
 * @returns LinceTrue if successful, and LinceFalse otherwise.
*/
LinceBool LinceSaveTilemapAsset(LinceAssetCache* cache, LinceSID asset_sid, string_t tileset);

/* ----------------------------------- */

/** @brief Initialise tilemap using settings and data pre-defined by the user,
* and provided via the passed handle.
*/
// LinceTilemap* LinceInitTilemap(LinceTilemap* map, uint32_t* grid);

/** @brief Delete memory allocated within the object, but not the object itself */
// void LinceUninitTilemap(LinceTilemap* tm);

/** @brief Simple helper function that iterates through tilemap->sprites
* calling `LinceDrawTilemap` on each.
* @param tm tilemap to draw
* @param shader Shader to use when rendering
*/
// void LinceDrawTilemap(LinceTilemap* tm, LinceShader* shader);

#endif /* LINCE_TILEMAP_H */