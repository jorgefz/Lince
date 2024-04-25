
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
    array_init(&map->indices, sizeof(uint32_t));
    array_resize(&map->indices, map_size);
    memcpy(map->indices.data, map_grid, sizeof(uint32_t)*map_size);

    // Generate grid of transforms
    array_init(&map->transforms, sizeof(LinceTransform));
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


/*
LinceTilemap* LinceInitTilemap(LinceTilemap* map, uint32_t* grid){

    // LINCE_ASSERT(map,               "NULL pointer");
    // LINCE_ASSERT(map->texture,      "Tileset undefined");
    // LINCE_ASSERT(map->cellsize[0]>0, "Cellsize must be greater than zero");
    // LINCE_ASSERT(map->cellsize[1]>0, "Cellsize must be greater than zero");
    // LINCE_ASSERT(map->width  > 0,    "Map width must be greater than zero");
    // LINCE_ASSERT(map->height > 0,    "Map height must be greater than zero");
    // LINCE_ASSERT(map->grid,          "Map grid undefined");
    // if(map->scale[0] < 1e-7f) map->scale[0] = 1.0f;
    // if(map->scale[1] < 1e-7f) map->scale[1] = 1.0f;

    // Initalise base sprite
    map->sprite = (LinceSprite){.texture = map->texture, .color = {1,1,1}};

    // Load tiles from texture
    map->texsize = (LincePoint){
        .x = (float)map->texture->width,
        .y = (float)map->texture->height
    };
    LinceTilesetGetCoords(map->texsize, map->cellsize, &map->tiles);

    // Ensure all indices in grid are valid
    uint32_t map_size = map->height * map->width;
    
    for(size_t i = 0; i != map_size; ++i){
        LINCE_ASSERT(grid[i] < map->tiles.size,
            "Invalid value in tilemap base grid "
            "(tile index %d but there are only %u tiles)",
            (int)grid[i], (int)map->tiles.size);
    }

    array_init(&map->indices, sizeof(uint32_t));
    array_resize(&map->indices, map_size);
    memcpy(map->indices.data, grid, sizeof(uint32_t)*map_size);

    // Generate grid of transforms
    array_init(&map->transforms, sizeof(LinceTransform));
    array_resize(&map->transforms, map->width * map->height);

    for(uint32_t y = 0; y != map->height; ++y){
        for(uint32_t x = 0; x != map->width; ++x){
            LinceTransform transform = map->base_transform;
            transform.x += (x - (float)(map->width )/2.0f) * transform.w;
            transform.y += (y - (float)(map->height)/2.0f) * transform.h;
            array_set(&map->transforms, &transform, map->width * y + x);    
        }
    }

    return map;
}

void LinceUninitTilemap(LinceTilemap* map){
    if(!map) return;
    array_uninit(&map->tiles);
    array_uninit(&map->transforms);
}


// TODO: don't draw tiles off screen!
void LinceDrawTilemap(LinceTilemap* map, LinceShader* shader){
    if(!map) return;
    for(uint32_t i = 0; i != map->width*map->height; ++i){
        LinceTransform* tr = array_get(&map->transforms, i);
        uint32_t idx = *(uint32_t*)array_get(&map->indices, i);
        LinceRect* uv = array_get(&map->tiles, idx);
        LinceDrawSpriteTile(&map->sprite, tr, uv, shader);
    }
}
*/

