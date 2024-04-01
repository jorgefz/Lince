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
typedef void* (*LinceAssetOnLoad)(const char* path);

///< Callback signature to free an asset from memory
typedef void (*LinceAssetOnUnload)(void* obj);

/// Callbacks to load, unload, and manage assets of a specific type
typedef struct LinceAssetCallbacks {
    LinceAssetOnLoad   load;   ///< Callback to load an asset from disk
    LinceAssetOnUnload unload; ///< Callback to free an asset from memory
} LinceAssetCallbacks;

/// Stores assets of a single type
typedef struct LinceAssetStore {
    int type;
    LinceAssetCallbacks callbacks;
    hashmap_t handles;
} LinceAssetStore;


typedef struct LinceAssetCache {
    char exedir[LINCE_PATH_MAX];      ///< Directory where the executable is located
    size_t exedir_length;             ///< Number of characters in the path of the executable
    char result_path[LINCE_PATH_MAX]; ///< Stores result of LinceFetchAssetPath

    /// Array of relative paths between the executable and the asset folders
    array_t folders;
    
    /// Stores info and handles for each asset type
    array_t stores; // array<LinceAssetStore> Cache stores for different asset types
    // LinceAssetStore stores[LinceAssetType_Count];

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

/** @brief Adds an existing asset to the cache
 * The asset must be heap-allocated. Passing it to the cache
 * will mean transfering ownership to it.
*/
void* LinceAssetCacheAdd(LinceAssetCache* cache, int type, const char* name, void* handle);

/** @brief Load an asset from memory */
void* LinceAssetCacheLoad(LinceAssetCache* cache, const char* name, int type);

/** @brief Unload a cached asset */
void LinceAssetCacheUnload(LinceAssetCache* cache, const char* name, int type);

/** @brief Retrieve a cached asset */
void* LinceAssetCacheGet(LinceAssetCache* cache, const char* name, int type);

// void* LinceAssetCacheLoadAsync(LinceAssetCache* cache, const char* name, int type);


#endif /* LINCE_ASSET_MANAGER_H */