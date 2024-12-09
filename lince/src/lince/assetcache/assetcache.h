#ifndef LINCE_ASSET_MANAGER_H
#define LINCE_ASSET_MANAGER_H

#include "lince/core/core.h"
#include "lince/containers/array.h"
#include "lince/containers/str.h"
#include "lince/containers/hashmap.h"


///< Callback signature to load an asset from disk
typedef void* (*LinceAssetLoad)(string_t path, void* args);

///< Callback signature to free an asset from memory
typedef void (*LinceAssetUnload)(void* obj);

/// Callbacks to load, unload, and manage assets of a specific type
typedef struct LinceAssetCallbacks {
    LinceAssetLoad   load;   ///< Callback to load an asset from disk
    LinceAssetUnload unload; ///< Callback to free an asset from memory
} LinceAssetCallbacks;

/// Stores assets of a single type
typedef struct LinceAssetStore {
    LinceAssetCallbacks callbacks; ///< Load and Unload functions for the asset type
    hashmap_t handles; /// Raw pointers to loaded assets, mapped by their filenames
                       /// A key may map to a NULL asset pointer if the asset was loaded and then unloaded.
} LinceAssetStore;


typedef struct LinceAssetCache {
    string_t exedir; ///< Directory where the executable is located
    array_t folders; /// array<string_t>, relative paths between the executable and the asset folders
    hashmap_t stores; /// hashmap<LinceAssetStore>, stores info and handles for each asset type
} LinceAssetCache;


/** @brief Initialise asset cache via provided pointer.
 * Must be deleted with `LinceUninitAssetCache`.
 * @returns LinceTrue if successful, and LinceFalse otherwise
 */
LinceBool LinceInitAssetCache(LinceAssetCache* cache);

/** @brief Uninitialise asset cache, freeing internally allocated memory */
void LinceUninitAssetCache(LinceAssetCache* cache);


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
 * @param cache Asset cache
 * @param path Path to the directory in which to search for assets.
 * @returns LinceTrue if the asset folder exists, LinceFalse otherwise.
 */
LinceBool LinceAssetCachePushFolder(LinceAssetCache* cache, string_t path);

/** @brief Retrieves the full path of an asset file by searching in the stored asset folders
* @param filename Location of the asset file within an asset folder
* @returns the full path of the asset; which needs to be freed with `string_free`.
*/
string_t LinceAssetCacheFetchPath(LinceAssetCache* cache, string_t filename);

/** @brief 
 * Registers a new type of asset on the asset cache.
 * @param cache Asset cache
 * @param name String identifier of the asset type. Must not be in use.
 * @param load Function to load the asset given a file path.
 * @param unload Function to free the asset from memory.
 * @returns Non-null pointer on success, and null otherwise.
*/
void* LinceAssetCacheAddType(LinceAssetCache* cache, string_t name, LinceAssetLoad load, LinceAssetUnload unload);

/** @brief Adds an existing asset to the cache
 * The asset must be heap-allocated.
 * Passing it to the cache will mean transfering ownership to it.
 * @param name String identifier for the asset
 * @param type String identifier of the asset type
 * @param handle Raw pointer to the (heap-allocated) asset data
 * @returns handle on success, and NULL otherwise
*/
void* LinceAssetCacheAdd(LinceAssetCache* cache, string_t name, string_t type, void* handle);

/** @brief Load an asset from memory
 * @param name String identifier for the asset
 * @param type String identifier of the asset type
 * @param args Custom argument passed to load function
 * @returns pointer to loaded asset, or NULL if asset does not exist or is already loaded.
*/
void* LinceAssetCacheLoad(LinceAssetCache* cache, string_t name, string_t type, void* args);

/** @brief Unload a cached asset
 * @param cache Asset cache
 * @param name String identifier of the asset
 * @param type String identifier of the asset type
 * @returns Non-null pointer if asset was successfully unloaded, and null otherwise.
*/
void* LinceAssetCacheUnload(LinceAssetCache* cache, string_t name, string_t type);

/** Reloads an existing asset from disk, discarding the previous one
 * @param name String identifier of the asset
 * @param type String identifier of the asset type
 * @param args Custom extra arguments for load function
 * @returns raw pointer to reloaded asset, or NULL if the asset does not exist.
 * @note If the asset wasn't previously loaded, it simply loads it.
*/
void* LinceAssetCacheReload(LinceAssetCache* cache, string_t name, string_t type, void* args);

/** @brief Retrieve a cached asset
 * @param name String identifier
 * @param type String identifier of the asset type
 * @returns Raw pointer to asset, or NULL if asset does not exist
 * @note If an asset is requested but is not currently loaded,
 * it will attempt to load it with no extra arguments (i.e. args = NULL in LinceAssetCacheLoad).
 * If you want to ensure an asset is loaded with specific arguments,
 * call LinceAssetCacheLoad once with the desired arguments, and then use LinceAssetCacheGet afterwards.
*/
void* LinceAssetCacheGet(LinceAssetCache* cache, string_t name, string_t type);

// void* LinceAssetCacheLoadAsync(LinceAssetCache* cache, const char* name, int type);


#endif /* LINCE_ASSET_MANAGER_H */