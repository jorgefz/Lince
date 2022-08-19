#include "lince/tiles/tilemap.h"


LinceTilemap* LinceCreateTilemap(LinceTilemap* props){

    LINCE_ASSERT(props, "Tilemap missing");
    LINCE_ASSERT(props->tileset, "Tileset missing");
    LINCE_ASSERT(props->tileset_count > 0, "Tileset must have more than zero tiles");
    LINCE_ASSERT(props->tiledata, "Tile data missing");
    LINCE_ASSERT(props->width > 0 && props->height > 0,
        "Tilemap size must be greater than zero");

    size_t tm_size = props->width * props->height;

    // Ensure no index in tile data is greater than the tileset size
    for(size_t i = 0; i != tm_size; ++i){
        LINCE_ASSERT(props->tiledata[i] < props->tileset_count,
            "Invalid value in tilemap data "
            "(tile index %d but there are only %d tiles)",
            (int)props->tiledata[i], (int)props->tileset_count);
    }

    // Allocate main data and copy settings
    LinceTilemap* tm = malloc(sizeof(LinceTilemap));
    LINCE_ASSERT_ALLOC(tm, sizeof(LinceTilemap));
    memmove(tm, props, sizeof(LinceTilemap));

    // Copy tile data
    tm->tiledata = malloc(sizeof(uint32_t) * tm_size);
    LINCE_ASSERT_ALLOC(tm->tiledata, sizeof(uint32_t) * tm_size);
    memmove(tm->tiledata, props->tiledata, sizeof(uint32_t) * tm_size);

    // Copy collision data
    tm->collision_data = malloc(sizeof(uint8_t) * tm_size);
    LINCE_ASSERT_ALLOC(tm->collision_data, sizeof(uint8_t) * tm_size);
    if(props->collision_data){
        memmove(tm->collision_data, props->collision_data, sizeof(uint8_t) * tm_size);
    } else {
        // set all to 0 (no collision)
        memset(tm->collision_data, (uint8_t)0, sizeof(uint8_t) * tm_size);
    }

    // NOTE: tileset is not copied!!

    return tm;
}



void LinceDeleteTilemap(LinceTilemap* tm){
    if(!tm) return;
    if(tm->tiledata) free(tm->tiledata);
    free(tm);
}
