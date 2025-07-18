
#include "lince/core/core.h"
#include "lince/core/logger.h"
#include "lince/utils/memory.h"
#include "lince/tiles/tilemap.h"
#include "lince/renderer/renderer.h"

LinceTilemap* LinceTilemapInit(LinceTilemap* map, uint32_t* map_grid){
    LINCE_ASSERT((map->width > 0) & (map->height > 0), "Invalid tilemap size");
    LINCE_ASSERT((map->width > 0) & (map->height > 0), "Invalid tilemap size");
    LINCE_ASSERT(map_grid, "Invalid tilemap grid");

    uint32_t map_size = map->width * map->height;
    map->tileset = NULL;
    map->sprite = (LinceSprite){ .color = {1,1,1} };

    // Copy grid of indices
    array_init_custom(&map->indices, sizeof(uint32_t), LINCE_DAST_ARRAY_ALLOCATOR);
    array_resize(&map->indices, map_size);
    memcpy(map->indices.data, map_grid, sizeof(uint32_t)*map_size);

    // Generate grid of transforms
    array_init_custom(&map->transforms, sizeof(LinceTransform), LINCE_DAST_ARRAY_ALLOCATOR);
    array_resize(&map->transforms, map_size);

    // Add overlap between map tiles to avoid
    // graphical artifacts at the edges between tiles
    const float overlap = map->scale.x * 0.01f;

    // Positions relative to the center of the map
    for(uint32_t x = 0; x != map->width; ++x){
        float xpos = ((float)x - (float)(map->width) /2.0f + 0.5f);
        
        for(uint32_t y = 0; y != map->height; ++y){    
            float ypos = ((float)y - (float)(map->height)/2.0f + 0.5f);
            LinceTransform transform = {
                .x = map->pos.x + xpos * map->scale.x,
                .y = map->pos.y + ypos * map->scale.y,
                .w = map->scale.x + overlap,
                .h = map->scale.y + overlap
            };
            array_set(&map->transforms, &transform, map->width * y + x);    
        }

    }
    return map;
}

void LinceTilemapUninit(LinceTilemap* map){
    array_uninit(&map->indices);
    array_uninit(&map->transforms);
}

void LinceTilemapUseTileset(LinceTilemap* map, LinceTileset* tileset){
    map->tileset = tileset;
    map->sprite.texture = tileset->texture;
}

// TODO: don't draw tiles off screen!
void LinceDrawTilemap(LinceTilemap* map, LinceShader* shader){
    if(!map->tileset){
        LINCE_WARN("No tileset available for drawing the tilemap");
        return;
    }

    for(uint32_t x = 0; x != map->width; ++x){
        for(uint32_t y = 0; y != map->height; ++y){
            uint32_t idx = map->width * y + x;
            LinceTransform* transform = array_get(&map->transforms, idx);
            uint32_t grid_idx = *(uint32_t*)array_get(&map->indices, idx);
            LinceRect* uv = array_get(&map->tileset->coords, grid_idx);
            if(!uv) {
                LINCE_WARN("Tileset does not have tile with index %u\n", grid_idx);
                continue;
            }
            LinceDrawSpriteTile(&map->sprite, transform, uv, shader);
        }
    }
}

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
void* LinceLoadTilemapAsset(LinceAssetCache* cache, string_t path, void* args){
    (void)args;
    
    string_t content = LinceReadFile(path);
	if(!string_ok(content)){
		LINCE_ERROR("Could not read tilemap at '%s'", path.str);
		return NULL;
	}

    char errbuf[256];
	toml_table_t* tab = toml_parse(content.str, errbuf, sizeof(errbuf));
    string_free(&content);
    if(!tab){
		LINCE_ERROR("Error parsing TOML file '%s'", path.str);
		LINCE_ERROR(" -> %s", errbuf);
	    return NULL;
    }

	toml_datum_t w    = toml_int_in(tab, "mapwidth");
	toml_datum_t h    = toml_int_in(tab, "mapheight");
	toml_datum_t cx   = toml_double_in(tab, "centerx");
    toml_datum_t cy   = toml_double_in(tab, "centery");
	toml_datum_t sw   = toml_double_in(tab, "scalewidth");
    toml_datum_t sh   = toml_double_in(tab, "scaleheight");
    toml_datum_t tset = toml_string_in(tab, "tileset");
    toml_array_t* grid = toml_array_in(tab, "grid");

    if(!w.ok || !h.ok || !grid){
        LINCE_ERROR("Error parsing TOML file '%s'", path.str);
        LINCE_ERROR(" -> Invalid parameters: %s%s%s",
            w.ok ? "" : "mapwidth, ",
            h.ok ? "" : "mapheight, ",
            grid ? "" : "grid"
        );
        toml_free(tab);
        return NULL;
    }

    uint32_t width  = (uint32_t)w.u.i;
    uint32_t height = (uint32_t)h.u.i;
    uint32_t nelem  = (uint32_t)toml_array_nelem(grid);
    if(nelem != width*height){
        LINCE_ERROR("Error parsing TOML file '%s'", path.str);
        LINCE_ERROR(" -> Expected %lu items but grid has %lu", width*height, nelem);
        toml_free(tab);
        return NULL;
    } else if ( (toml_array_kind(grid) != 'v') || (toml_array_type(grid) != 'i') ){
        LINCE_ERROR("Error parsing TOML file '%s'", path.str);
        LINCE_ERROR(" -> Grid values should be integers");
        toml_free(tab);
        return NULL;
    }

    uint32_t* grid_data = LinceAlloc(sizeof(uint32_t) * nelem);
    for(uint32_t i = 0; i != nelem; ++i){
        grid_data[i] = (uint32_t)toml_int_at(grid, i).u.i;
    }
    
    LincePoint pos, scale;
    pos.x   = cx.ok ? (float)cx.u.d : 0.0f;
    pos.y   = cy.ok ? (float)cy.u.d : 0.0f;
    scale.x = sw.ok ? (float)sw.u.d : 1.0f;
    scale.y = sh.ok ? (float)sh.u.d : 1.0f;
    
    LinceTilemap* map = LinceCalloc(sizeof(LinceTilemap));
    map->width  = width;
    map->height = height;
    map->pos    = pos;
    map->scale  = scale;

    LinceTilemap *success = LinceTilemapInit(map, grid_data);
    if(!success){
        toml_free(tab);
        return NULL;
    }

    if (tset.ok){
        string_t tset_str = string_scoped(tset.u.s, strlen(tset.u.s));
        LinceSID tset_sid = LinceMakeSID(tset_str);
        LinceTileset* tileset = LinceAssetCacheGet(cache, tset_sid);
        if(!tileset){
            LinceAssetCacheRegister(cache, tset_sid, LinceSIDFromLit("tileset"), tset_str);
            tileset = LinceAssetCacheGet(cache, tset_sid);
        }
        if (tileset) LinceTilemapUseTileset(map, tileset);
    }

    toml_free(tab);
    return map;
}

/** @brief Unload tileset loaded with `LinceLoadTilesetAsset` */
void LinceUnloadTilemapAsset(LinceAssetCache* cache, void* obj){
	if(!cache || !obj) return;
	LinceTilemapUninit(obj);
}

/** @brief Write tilemap data to disk */
LinceBool LinceSaveTilemapAsset(LinceAssetCache* cache, LinceSID asset_sid, string_t tileset){
    if(!cache) return LinceFalse;

    LinceTilemap* map = LinceAssetCacheGet(cache, asset_sid);
    string_t path = LinceAssetCacheGetPath(cache, asset_sid);
    if(!map || !string_ok(path)) return LinceFalse;

    FILE* f = fopen(path.str, "w");

    fprintf(f, "mapwidth    = %lu\n", map->width);
    fprintf(f, "mapheight   = %lu\n", map->height);
    fprintf(f, "centerx     = %f\n", map->pos.x);
    fprintf(f, "centery     = %f\n", map->pos.y);
    fprintf(f, "scalewidth  = %f\n", map->scale.x);
    fprintf(f, "scaleheight = %f\n", map->scale.y);
    fprintf(f, "tileset     = %s",   tileset.str);

    fprintf(f, "grid        = [");
    for(uint32_t* i = map->indices.begin; i != map->indices.begin; ++i){
        if((*i % map->width) == 0){
            fprintf(f, "\n    ");
        }
        fprintf(f, "%lu,", *i);
    }
    fprintf(f,"\n]");

    fclose(f);
    return LinceTrue;
}
