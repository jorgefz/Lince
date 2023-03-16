
#include "lince/core/core.h"
#include "lince/core/memory.h"
#include "lince/tiles/tilemap.h"
#include "lince/renderer/renderer.h"

void LinceInitTilemap(LinceTilemap* map){
    LINCE_ASSERT(map,               "NULL pointer");
    LINCE_ASSERT(map->texture,      "Tileset undefined");
    LINCE_ASSERT(map->cellsize[0]>0, "Cellsize must be greater than zero");
    LINCE_ASSERT(map->cellsize[1]>0, "Cellsize must be greater than zero");
    LINCE_ASSERT(map->width  > 0,    "Map width must be greater than zero");
    LINCE_ASSERT(map->height > 0,    "Map height must be greater than zero");
    LINCE_ASSERT(map->grid,          "Map grid undefined");
    if(map->scale[0] < 1e-7f) map->scale[0] = 1.0f;
    if(map->scale[1] < 1e-7f) map->scale[1] = 1.0f;

    // Load tiles from texture
    LinceGetTilesFromTexture(map->texture, map->cellsize, &map->tiles);

    // Ensure all indices in grid are valid
    size_t map_size = map->height * map->width;
    for(size_t i = 0; i != map_size; ++i){
        LINCE_ASSERT(map->grid[i] < map->tiles.size,
            "Invalid value in tilemap base grid "
            "(tile index %d but there are only %u tiles)",
            (int)map->grid[i], (int)map->tiles.size);
    }

    // Generate sprite grid
    array_init(&map->sprites, sizeof(LinceSprite));
    // array_resize(&map->sprites, map_size);
    float sw = map->scale[0], sh = map->scale[1];
    float w = (float)map->width, h = (float)map->height;

    for(size_t j = 0; j != map->height; ++j){
        for(size_t i = 0; i != map->width; ++i){
            LinceSprite sprite = {
                .x = (float)i * map->scale[0] - w*sw/2.0f + map->offset[0],
                .y = (float)j * map->scale[1] - h*sh/2.0f + map->offset[1],
                .w = map->scale[0], .h = map->scale[1],
                .color = {1,1,1,1}, .zorder = map->zorder,
                .texture = map->texture,
                .tile = array_get(&map->tiles, map->grid[j*map->width + i])
            };
            array_push_back(&map->sprites, &sprite);
        }
    }
}

void LinceUninitTilemap(LinceTilemap* map){
    if(!map) return;
    array_uninit(&map->tiles);
    array_uninit(&map->sprites);
}


void LinceDrawTilemap(LinceTilemap* map, LinceShader* shader){
    if(!map) return;
    for(size_t i = 0; i != map->sprites.size; ++i){
        LinceSprite* sp = array_get(&map->sprites, i);
        LinceDrawSprite(sp, shader);
    }
}


