
#include "lince/core/core.h"
#include "lince/core/memory.h"
#include "lince/tiles/tilemap.h"
#include "lince/renderer/renderer.h"

LinceTilemap* LinceCreateTilemap(LinceTilemap* props){
    // BEWARE: tileset is not copied!!

    LINCE_ASSERT(props, "Tilemap missing");
    LINCE_ASSERT(props->tileset, "Tileset missing");
    LINCE_ASSERT(props->tileset_size > 0, "Tileset must have more than zero tiles");
    LINCE_ASSERT(props->base_grid, "Tile data missing");
    LINCE_ASSERT(props->width > 0 && props->height > 0,
        "Tilemap size must be greater than zero");

    size_t tm_size = props->width * props->height;

    // Ensure no index in the grid is greater than the number of tiles in the tileset
    for(size_t i = 0; i != tm_size; ++i){
        LINCE_ASSERT(props->base_grid[i] < props->tileset_size,
            "Invalid value in tilemap base grid "
            "(tile index %d but there are only %d tiles)",
            (int)props->base_grid[i], (int)props->tileset_size);

        if(!props->bkg_grid) continue;

        LINCE_ASSERT(props->bkg_grid[i] < props->tileset_size,
            "Invalid value in tilemap bkg grid "
            "(tile index %d but there are only %d tiles)",
            (int)props->bkg_grid[i], (int)props->tileset_size);
    }

    // Allocate main data and copy settings
    LinceTilemap* tm = LinceNewCopy(props, sizeof(LinceTilemap));

    // Copy base and background grids
    tm->base_grid = LinceNewCopy(props->base_grid, sizeof(uint32_t) * tm_size);
    if(props->bkg_grid){
        tm->bkg_grid = LinceNewCopy(props->bkg_grid, sizeof(uint32_t) * tm_size);
    } else {
        tm->bkg_grid = NULL; // Can't guarantee tile 0 is no texture.
    }

    // Copy logic data
    if(props->logic_grid){
        tm->logic_grid = LinceNewCopy(props->logic_grid, sizeof(uint8_t) * tm_size);
    } else {
        tm->logic_grid = LinceCalloc(sizeof(uint8_t) * tm_size);
    }

    // Copy overlay tiles
    if(!props->overlay_positions ||
       !props->overlay_tiles ||
        props->overlay_count == 0){
        return tm;
    }

    tm->overlay_tiles = LinceNewCopy(
        props->overlay_tiles,
        sizeof(LinceTile) * tm->overlay_count
    );

    tm->overlay_positions = LinceNewCopy(
        props->overlay_positions,
        sizeof(vec2) * tm->overlay_count
    );

    return tm;
}


void LinceDeleteTilemap(LinceTilemap* tm){
    if(!tm) return;
    LinceFree(tm->base_grid);
    LinceFree(tm->logic_grid);
    if(tm->bkg_grid) LinceFree(tm->bkg_grid);
    if(tm->overlay_tiles) LinceFree(tm->overlay_tiles);
    if(tm->overlay_positions) LinceFree(tm->overlay_positions);
    LinceFree(tm);
}


static void LinceDrawTilemapGrids(LinceTilemap* tm){
    // Draw base & bkg tiles
    for(size_t j = 0; j != tm->height; ++j){
        for(size_t i = 0; i != tm->width; ++i){
            LinceQuadProps quad = {
                .x = (float)i - tm->offset[0],
                .y = (float)j - tm->offset[1],
                .w = 1.001f, .h = 1.001f,
                .color = {1,1,1,1},
                .tile = &tm->tileset[tm->base_grid[j*tm->width + i]],
                .zorder = LINCE_TILEMAP_Z
            };
            LinceDrawQuad(quad); // base tile

            if(!tm->bkg_grid) continue;
            quad.tile = &tm->tileset[tm->bkg_grid[j*tm->width + i]];
            quad.zorder += 0.001f; // slightly above base grid
            LinceDrawQuad(quad); // bkg tile
        }
    }

}


static void LinceDrawTilemapOverlays(LinceTilemap* tm){
    if(!tm || !tm->overlay_tiles|| !tm->overlay_positions){
        return;
    }

    for(size_t i = 0; i != tm->overlay_count; ++i){
        LinceTile* tile = &tm->overlay_tiles[i];
        float z = LinceYSortedZ(
            tm->overlay_positions[i][1] - tm->offset[1] - tile->tilesize[1]/2.0f,
            (vec2){-100.0f, 100.0f},
            (vec2){0.1f, 0.9f}
        );
        LinceDrawQuad((LinceQuadProps){
            .x = tm->overlay_positions[i][0] - tm->offset[0],
            .y = tm->overlay_positions[i][1] - tm->offset[1],
            .w = tile->tilesize[0],
            .h = tile->tilesize[1],
            .color = {1,1,1,1},
            .tile = tile,
            .zorder = z
        });
    }
}


void LinceDrawTilemap(LinceTilemap* tm){
    if(!tm) return;
    LinceDrawTilemapGrids(tm);
    LinceDrawTilemapOverlays(tm);
}

