#ifndef LINCE_ASSET_MANAGER_H
#define LINCE_ASSET_MANAGER_H

#include "lince/core/core.h"
#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"

enum LinceAssetType {
    LinceAsset_None = -1,
    LinceAsset_Image,
    LinceAsset_Texture,
    LinceAsset_TextFile,
    LinceAsset_ShaderHeader,
    LinceAsset_ShaderProgram,
    LinceAsset_Font,
    LinceAsset_Misc, // Generic files
    LinceAsset_Count // Number of asset types
};

typedef struct LinceAssetInfo {
    // uuid?
    int type;
    void* handle;
    LinceBool loaded;           ///< Whether the resource is loaded
    char path[LINCE_PATH_MAX];  ///< Full path to the resource
    char name[LINCE_NAME_MAX];  ///< Filename of the resource
} LinceAssetInfo;

// Stores data for assets of the same type
typedef struct LinceAssetStore {
    int type;
    array_t info;            ///<
    hashmap_t info_indices;  ///< 
    hashmap_t handles; ///< Maps name to index
    // ctor & dtor functions specific to this type
} LinceAssetStore;

typedef struct LinceAssetCache {
    char exedir[LINCE_PATH_MAX];     ///< Directory where the executable is located
    size_t exedir_length;
    char result_path[LINCE_PATH_MAX]; ///< Stores result of LinceFetchAssetPath
    array_t folders;            ///< Array of relative paths between executable and asset folders
    array_t stores; /**< Stores asset info and handles, indexed by enum LinceAssetType */
    
} LinceAssetCache;

/*
cache = LinceAssetCacheInit()

LinceAssetCachePushFolder(cache, "path_to_my_assets_folder");
LinceAssetCachePushFolder(cache, "path_to_my_second_assets_folder");

LinceTexture* player = LinceAssetCacheGet(cache, "textures/player.png", LinceAsset_Texture);

LinceAssetCacheUninit(cache);

// Load an asset using a switch and its appropriate function
void* LinceAssetCacheLoad(LinceAssetCache* cache, const char* filename, int type);

// Fetch an asset. If not cached, load it.
void* LinceAssetCacheGet(LinceAssetCache* cache, const char* name, int type);

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

void LinceAssetCacheLoad(LinceAssetCache* cache, const char* filename, int type);


#endif /* LINCE_ASSET_MANAGER_H */