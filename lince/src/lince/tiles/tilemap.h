#ifndef LINCE_TILEMAP_H
#define LINCE_TILEMAP_H

#include "lince/tiles/tileset.h"

// Z value at which to draw the base grid of tilemaps
#ifndef LINCE_TILEMAP_Z
#define LINCE_TILEMAP_Z 0.0f
#endif

/* Flags for Tilemap logic grid */
typedef enum LinceTilemapBlockFlags{
    LinceTilemap_Empty   = 0x0,
    
    /* Flags for collision on the four corners of a tile */
    LinceTilemap_SolidUL = 0x1, // upper left  corner
    LinceTilemap_SolidUR = 0x2, // upper right corner
    LinceTilemap_SolidLL = 0x4, // lower left  corner
    LinceTilemap_SolidLR = 0x8, // lower right corner
    LinceTilemap_SolidTop   = LinceTilemap_SolidUL | LinceTilemap_SolidUR,   // top half
    LinceTilemap_SolidBot   = LinceTilemap_SolidLL | LinceTilemap_SolidLR,   // bottom half
    LinceTilemap_SolidLeft  = LinceTilemap_SolidUL | LinceTilemap_SolidLL,   // left half
    LinceTilemap_SolidRight = LinceTilemap_SolidUR | LinceTilemap_SolidLR,   // right half
    LinceTilemap_Solid      = LinceTilemap_SolidTop | LinceTilemap_SolidBot, // fully solid
} LinceTilemapBlockFlags;


typedef struct LinceTilemap{
    LinceTile* tileset;
    size_t tileset_size; // number of tiles in the tileset

    vec2 offset; // position offset from world origin
    vec2 scale; // scale of individual tiles - default is (1,1)

    size_t width, height;  // dimensions of the map
    uint32_t* base_grid;   // main tile map
    uint8_t* logic_grid;   // collision map, etc
    uint32_t* bkg_grid;    // background tiles without normZ on top of base_grid

    // List of tile objects drawn with normalized Z
    LinceTile* overlay_tiles;
    vec2* overlay_positions;
    size_t overlay_count;

    /*
    CallbackFn on_teleport(this, target-tilemap, trigger, ...)
    */
    
} LinceTilemap;


LinceTilemap* LinceCreateTilemap(LinceTilemap* props);

void LinceDeleteTilemap(LinceTilemap* tm);

void LinceDrawTilemap(LinceTilemap* tm);

#endif /* LINCE_TILEMAP_H */