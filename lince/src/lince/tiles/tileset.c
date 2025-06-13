#include "tiles/tileset.h"

/** @brief Calculates the texture coordinates of all cells in a tileset.
 * @param tileset Tileset
*/
static void LinceTilesetCalculateCoords(LinceTileset* tileset){
	float cellw = tileset->cellsize.x / tileset->texsize.x;
	float cellh = tileset->cellsize.y / tileset->texsize.y;
	uint32_t xcells = tileset->xcells;
	uint32_t ycells = tileset->ycells;
	
	array_init_custom(&tileset->coords, sizeof(LinceRect), LINCE_DAST_ARRAY_ALLOCATOR);
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
void* LinceLoadTilesetAsset(LinceAssetCache* cache, string_t path, void* args){
	(void)args;
	
	string_t content = LinceReadFile(path);
	if(!string_ok(content)){
		LINCE_ERROR("Could not read tileset at '%s'", path.str);
		return NULL;
	}

	char errbuf[256];
	toml_table_t* tab = toml_parse(content.str, errbuf, sizeof(errbuf));
    if(!tab){
		LINCE_ERROR("Failed to parse TOML file");
		LINCE_ERROR("%s", errbuf);
		string_free(&content);
        return NULL;
    }
	string_free(&content);

	toml_datum_t texpath = toml_string_in(tab, "texture");
	toml_datum_t cellw   = toml_int_in(tab, "tilewidth");
	toml_datum_t cellh   = toml_int_in(tab, "tileheight");
	
	if (!texpath.ok || !cellw.ok || !cellh.ok){
		toml_free(tab);
		LINCE_ERROR("Failed to parse tileset TOML file - invalid parameters");
		LINCE_ERROR("  texture: '%s', cellw: %d, cellh: %d",
				texpath.ok ? texpath.u.s    : "NULL",
				cellw.ok   ? (int)cellw.u.i : -1,
				cellh.ok   ? (int)cellh.u.i : -1
		);
		return NULL;
	}

	string_t texpath_str = string_scoped(texpath.u.s, strlen(texpath.u.s));
	LinceSID texsid = LinceMakeSID(texpath_str);
    LinceTexture* tex = LinceAssetCacheGet(cache, texsid);
	if(!tex){
		LinceAssetCacheRegister(cache, texsid, LinceSIDFromLit("texture"), texpath_str);
		tex = LinceAssetCacheGet(cache, texsid);
	}

	LinceTileset* tset = LinceCalloc(sizeof(LinceTileset));
	LinceTilesetInit(tset, tex, (uint32_t)cellw.u.i, (uint32_t)cellh.u.i);
	toml_free(tab);
	return tset;
}

/** @brief Unload tileset loaded with `LinceLoadTilesetAsset` */
void LinceUnloadTilesetAsset(LinceAssetCache* cache, void* obj){
	if(!cache || !obj) return;
	LinceTilesetUninit(obj);
}
