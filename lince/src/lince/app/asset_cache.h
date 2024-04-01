#ifndef LINCE_ASSET_MANAGER_H
#define LINCE_ASSET_MANAGER_H

#include "lince/core/core.h"
#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"


typedef enum LinceAssetType {
    LinceAssetType_None = -1,
    LinceAssetType_Image = 0, // p = Load(p, path)
    LinceAssetType_Texture,   // p = Load(path, flags) 
    // LinceAsset_TextFile,   // p = Load(path);
    // LinceAsset_ShaderHeader,  // 
    // LinceAsset_ShaderProgram, // p = Load(path1, path2)
    // LinceAsset_Font,          // p = Load(path, sizes[])
    // LinceAsset_Misc, // Generic files
    LinceAssetType_Count // Number of asset types
} LinceAssetType;


///< Callback signature to load an asset from disk
typedef void* (*LinceAssetOnLoad)(const char* path, void* args);

///< Callback signature to free an asset from memory
typedef void (*LinceAssetOnUnload)(void* obj);

/// Callbacks to load, unload, and manage assets of a specific type
typedef struct LinceAssetCallbacks {
    LinceAssetOnLoad   load;   ///< Callback to load an asset from disk
    LinceAssetOnUnload unload; ///< Callback to free an asset from memory
} LinceAssetCallbacks;

/// Stores assets of a single type
typedef struct LinceAssetStore {
    int type; ///< Index in the stores array
    LinceAssetCallbacks callbacks; ///< Load and Unload functions

    /// Asset names mapped to their raw pointers.
    /// The map may have a key storing a NULL asset if said asset was loaded and then unloaded.
    hashmap_t handles; 
} LinceAssetStore;


typedef struct LinceAssetCache {
    char exedir[LINCE_PATH_MAX];      ///< Directory where the executable is located
    size_t exedir_length;             ///< Number of characters in the path of the executable
    char result_path[LINCE_PATH_MAX]; ///< Stores result of LinceFetchAssetPath

    /// Array of relative paths between the executable and the asset folders
    array_t folders;
    
    /// array<LinceAssetStore> - Stores info and handles for each asset type
    array_t stores;

} LinceAssetCache;


/** @brief Create an asset cache
 * @returns Valid pointer to initialised asset cache, or NULL if unsuccessful.
 */
LinceAssetCache* LinceCreateAssetCache();

/** @brief Deletes an asset cache
 * Unloads and frees all assets it holds.
 * @param cache Cache to delete. Must be the return value of `LinceCreateAssetCache`.
 */
void LinceDeleteAssetCache(LinceAssetCache* cache);


/** @brief Saves the location of an asset folder to the list of search paths.
* The path must be relative to the location of the executable.
* @param dir Zero-terminated string with the path to an asset folder to store.
* @returns LinceTrue if the asset folder exists, LinceFalse otherwise.
*/
LinceBool LinceAssetCachePushFolder(LinceAssetCache* cache, const char* folder);

/** @brief Retrieves the full path of an asset file by searching in the stored asset folders
* @param filename Location of the asset file within an asset folder
* @returns a pointer to the resulting path, or NULL if the asset could not be found
* @note the path returned is saved internally and will be overwritten the next time the function is called.
*/
char* LinceAssetCacheFetchPath(LinceAssetCache* cache, const char* filename);

// int LinceAssetCacheAddType(LinceAssetCache* cache, LinceAssetOnLoad load, LinceAssetOnUnload unload);

/** @brief Adds an existing asset to the cache
 * The asset must be heap-allocated. Passing it to the cache will mean transfering ownership to it.
 * @param name String identifier for the asset
 * @param type Type of the input asset
 * @param handle Raw pointer to the (heap-allocated) asset data
 * @returns handle on success, and NULL otherwise
*/
void* LinceAssetCacheAdd(LinceAssetCache* cache, const char* name, int type, void* handle);

/** @brief Load an asset from memory
 * @param name String identifier for the asset
 * @param type Type of the asset to load
 * @param args Custom argument passed to load function
 * @returns pointer to loaded asset, or NULL if asset does not exist or is already loaded.
*/
void* LinceAssetCacheLoad(LinceAssetCache* cache, const char* name, int type, void* args);

/** @brief Unload a cached asset
 * @param name String identifier of the asset
 * @param type Type of the asset to unload
*/
void LinceAssetCacheUnload(LinceAssetCache* cache, const char* name, int type);

/** Reloads an existing asset from disk, discarding the previous one
 * @param name String identifier of the asset
 * @param type Asset type
 * @param args Custom extra arguments for load function
 * @returns raw pointer to reloaded asset, or NULL if the asset does not exist.
 * @note If the asset wasn't previously loaded, it simply loads it.
*/
void* LinceAssetCacheReload(LinceAssetCache* cache, const char* name, int type, void* args);

/** @brief Retrieve a cached asset
 * @param name String identifier
 * @param type Asset type
 * @returns Raw pointer to asset, or NULL if asset does not exist
 * @note If an asset is requested but is not currently loaded,
 * it will attempt to load it with no extra arguments (i.e. args = NULL in LinceAssetCacheLoad).
 * If you want to ensure an asset is loaded with specific arguments,
 * call LinceAssetCacheLoad once with the desired arguments, and then use LinceAssetCacheGet afterwards.
*/
void* LinceAssetCacheGet(LinceAssetCache* cache, const char* name, int type);

// void* LinceAssetCacheLoadAsync(LinceAssetCache* cache, const char* name, int type);


#endif /* LINCE_ASSET_MANAGER_H */