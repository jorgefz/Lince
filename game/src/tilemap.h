#ifndef LINCE_TILEMAP_H
#define LINCE_TILEMAP_H

#include "lince/tiles/tileset.h"

typedef struct LinceTilemap{
    LinceTile* tileset;
    size_t tileset_count;

    size_t width, height;
    vec2 offset;
    uint32_t* tiledata;
    uint8_t* collision_data;

    LinceTile* overlay_tiles;
    vec2* overlay_positions;
    size_t overlay_count;

    /*
    CallbackFn on_teleport(this, target-tilemap, trigger, ...)

    */
    
} LinceTilemap;


LinceTilemap* LinceCreateTilemap(LinceTilemap* props);

void LinceDeleteTilemap(LinceTilemap* tm);

#endif /* LINCE_TILEMAP_H */