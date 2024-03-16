#ifndef LINCE_ASSET_MANAGER_H
#define LINCE_ASSET_MANAGER_H

#include "lince/core/core.h"
#include "lince/containers/array.h"

/*
enum LinceAssetType {
    LinceAsset_None = -1,
    LinceAsset_Image,
    LinceAsset_Texture,
    LinceAsset_TextFile,
    LinceAsset_ShaderHeader,
    LinceAsset_ShaderProgram,
    LinceAsset_Font,
    LinceAsset_Misc // Generic files
};


// Contains all the cached assets of the same type
// Useful because all should have the same size
struct LinceAssetStore {
    array_t paths;
    array_t names;
    int type;
    array_t data;
    // Custom arguments for textures loading?
    // Default asset in case requested one is not found?
};

*/

typedef struct LinceAssetCache {
    char exe_dir[LINCE_PATH_MAX];   ///< Directory where the executable is located
    array_t asset_folders;          ///< Array of relative paths between executable and asset folders
    char result_path[LINCE_PATH_MAX]; ///< Stores result of LinceFetchAssetPath

    // Maps resource name (e.g. "textures/player.png") to an asset pointer.
    // If it's not loaded, it loads it into memory and caches it.
    // hashmap_t name_map;

    // Maps an asset type name to its enum value,
    // e.g. "textures" -> LinceAssetType_Texture
    // Created at initialisation
    // hashmap_t type_map;

    // Stores raw asset data
    // array<LinceAssetStore>
    // e.g. textures_store = stores[LinceAssetType_Texture];
    // array_t stores;           

} LinceAssetCache;

/*
cache = LinceAssetCacheInit()

LinceAssetCachePushFolder(cache, "path_to_my_assets_folder");
LinceAssetCachePushFolder(cache, "path_to_my_second_assets_folder");

LinceTexture* player = LinceAssetCacheGet(cache, "textures/player.png");

LinceAssetCacheUninit(cache);


// Get the folder name of an asset type from its enum value
static const char* LinceAssetCacheGetTypeName(int type){
    switch(type){
        case LinceAsset_Texture: return "textures";
        default:                 return NULL;
    };
}


// Identify the type of asset from its name, e.g. "textures/player.png" is a TextureType
static int LinceAssetCacheFindType(LinceAssetCache* cache, const char* filename);

// Load an asset using a switch and its appropriate function
static void* LinceAssetCacheLoad(LinceAssetCache* cache, const char* filename);

// Fetch an asset. If not cached, load it.
void* LinceAssetCacheGet(LinceAssetCache* cache, const char* name){
    void* asset = hashmap_get(&cache->name_map, name);
    if(!asset){
        // Attempt to load it
        asset = LinceAssetCacheLoad(cache, name);
        if(!asset){
            // Emit warning
            return NULL;
        }
    }
    return asset;
}

*/



/** @brief Initialise asset cache */
void LinceInitAssetCache(LinceAssetCache* cache);

/** @brief Uninitialise asset cache, freeing internally allocated memory */
void LinceUninitAssetCache(LinceAssetCache* cache);

/** @brief Saves the location of an asset folder to the list of search paths.
* The path must be relative to the location of the executable.
* @param dir Zero-terminated string with the path to an asset folder to store.
* @returns LinceTrue if the asset folder exists, LinceFalse otherwise.
*/
LinceBool LinceAssetCachePushFolder(LinceAssetCache* cache, const char* dir);

/** @brief Retrieves the full path of an asset file by searching in the stored asset folders
* @param filename Location of the asset file within an asset folder
* @returns a pointer to the resulting path, or NULL if the asset could not be found
* @note the path returned is saved internally and will be overwritten the next time the function is called.
*/
char* LinceAssetCacheFetchPath(LinceAssetCache* cache, const char* filename);


#endif /* LINCE_ASSET_MANAGER_H */