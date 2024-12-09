#ifndef SANDBOX_UTIL_H
#define SANDBOX_UTIL_H

#include <lince.h>

LinceTileset* LinceLoadTilesetToml(LinceAssetCache* assets, string_t fname);
LinceTilemap* LinceLoadTilemapToml(LinceAssetCache* assets, string_t fname);

#endif /* SANDBOX_UTIL_H */