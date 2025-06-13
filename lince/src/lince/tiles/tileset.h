#ifndef LINCE_TILESET_H
#define LINCE_TILESET_H

#include "lince/core/core.h"
#include "lince/renderer/transform.h"
#include "lince/renderer/texture.h"
#include "lince/utils/fileio.h"
#include "lince/assetcache/assetcache.h"
#include "toml.h"


typedef struct LinceTileset {
	LinceTexture* texture;	///< Base texture atlas
	LincePoint    texsize;  ///< Size of the base texture in pixels
	LincePoint    cellsize;	///< Size of a tile in pixels
	uint32_t      xcells;   ///< Number of cells on X direction in tileset
	uint32_t      ycells;   ///< Number of cells on Y direction in tileset
	array_t       coords;   ///< Texture coordinates of all cells in the tileset
} LinceTileset;

/** @brief Initialise a tileset from a texture */
LinceTileset* LinceTilesetInit(LinceTileset* tileset, LinceTexture* tex, uint32_t cell_width, uint32_t cell_height);

/** @brief Free memory allocated by tileset */
void LinceTilesetUninit(LinceTileset* tset);

/** @brief Return the UV texture coordinates of a tile
 * at position (x,y) in a tileset, from the lower left corner.
 * @param tileset Tileset
 * @param x X location of the tile in the tileset in units of cells.
 * @param y Y location of the tile in the tileset in units of cells.
*/
LinceRect* LinceTilesetGetTileCoords(LinceTileset* tileset, uint32_t x, uint32_t y);

/** @brief Initialise a tileset from a TOML file.
 * TOML file must have the following parameters:
 * 	texture     :(str) Path of base texture
 *  tilewidth   :(int) Tile width in pixels
 *  tileheight  :(int) Tile height in pixels
 * @param cache Asset cache
 * @param path  Path to tileset TOML file
 * @param args  Unused
 * @returns Pointer to allocated initalised tileset,
 *   or NULL if TOML file or texture could not be loaded.
*/
void* LinceLoadTilesetAsset(LinceAssetCache* cache, string_t path, void* args);

/** @brief Unload tileset loaded with `LinceLoadTilesetAsset` */
void LinceUnloadTilesetAsset(LinceAssetCache* cache, void* obj);

#endif /* LINCE_TILESET_H */