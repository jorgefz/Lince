#ifndef LINCE_ASSET_MANAGER_H
#define LINCE_ASSET_MANAGER_H

#include "toml.h"
#include "lince/core/core.h"
#include "lince/assetcache/sid.h"

/// Forward declaration
struct LinceAssetCache;

///< Callback signature to load an asset from disk
typedef void* (*LinceAssetLoad)(struct LinceAssetCache *cache, string_t path, void* args);

///< Callback signature to free an asset from memory
typedef void (*LinceAssetUnload)(struct LinceAssetCache *cache, void* obj);

/// Callbacks to load, unload, and manage assets of a specific type
typedef struct LinceAssetLoader {
    LinceAssetLoad   load;   ///< Callback to load an asset from disk
    LinceAssetUnload unload; ///< Callback to free an asset from memory
    LinceSID default_asset;  ///< Default asset to use when a requested one does not exist
} LinceAssetLoader;

typedef struct LinceAsset {
    LinceSID sid;   ///< String ID for the asset name
    LinceSID type;  ///< String ID for the asset type
    string_t path;  ///< Path to asset
    void* handle;   ///< Opaque pointer to loaded asset
} LinceAsset;

typedef struct LinceAssetCache {
    string_t exedir;  ///< Directory where the executable is located
    array_t folders;  ///< array<string_t>, relative paths between the executable and the asset folders
    hashmap_t assets; ///< hashmap<LinceSID, LinceAsset>, stores info and handles for each asset.
    hashmap_t types;  ///< hashmap<LinceSID, LinceAssetLoader>, stores callbacks for asset types.
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
 * @note This is probably not needed
 */
LinceAssetCache* LinceCreateAssetCache();

/** @brief Deletes an asset cache
 * Unloads and frees all assets it holds.
 * @param cache Cache to delete. Must be the return value of `LinceCreateAssetCache`.
 * @note This is probably not needed
 */
void LinceDeleteAssetCache(LinceAssetCache* cache);


/** @brief Saves the location of an asset folder to the list of search paths.
 * The path must be relative to the location of the executable.
 * @param cache Asset cache
 * @param path Path to the directory in which to search for assets.
 * @returns LinceTrue if the asset folder exists, LinceFalse otherwise.
 */
LinceBool LinceAssetCachePushPath(LinceAssetCache* cache, string_t path);

/** @brief Retrieves the full path of an asset file by searching in the stored asset folders
* @param filename Location of the asset file within an asset folder
* @returns the full path of the asset; which needs to be freed with `string_free`,
*          or, on failure, an invalid string for which `string_ok` returns LinceFalse.
*/
string_t LinceAssetCacheFindPath(LinceAssetCache* cache, string_t filename);

/** @brief Registers a new type of asset on the asset cache.
 * An asset type defines an interface to load and unload assets of the same kind.
 * If type already exists, it's loader is updated to the input functions.
 * @param cache  Asset cache
 * @param name   String ID of the asset type. Must not be in use.
 * @param load   Function to load the asset given a file path.
 * @param unload Function to free the asset from memory.
 * @returns LinceTrue if type was successfully added and LinceFalse otherwise.
*/
LinceBool LinceAssetCacheAddType(LinceAssetCache* cache, LinceSID name, LinceAssetLoad load, LinceAssetUnload unload);

/** @brief Registers an asset to the cache. Stores its type and filename, but does not load it
 * @param cache Asset cache
 * @param sid   String ID of the asset
 * @param type  String ID of its type
 * @param path  Location of the asset inside an asset folder.
 * @returns LinceTrue if asset was succesfully registered, and LinceFalse otherwise.
*/
LinceBool LinceAssetCacheRegister(LinceAssetCache* cache, LinceSID sid, LinceSID type, string_t path);

/** @brief Registers assets predefined in an 'index' TOML file.
 * @param cache Asset cache
 * @param path  Location of asset index file
 * @returns LinceTrue if all assets were succesfully registered, and LinceFalse otherwise.
 * @note The 'asset index' file is essentially a list of TOMl tables with three items:
 * 'name', 'type', 'path'. The 'name' is an unique string name from which the asset SID will be computed.
 * The 'type' is the asset type that must have already been added with `LinceAssetCacheAddType()`.
 * The 'path' is the path to the asset file relative to the asset folder.
*/
LinceBool LinceAssetCacheRegisterIndex(LinceAssetCache* cache, string_t index_path);

/** @brief Adds an existing asset to the cache
 * The asset must be heap-allocated.
 * Passing it to the cache will mean transfering ownership to it, so don't free it yourself afterwards!
 * @param cache  Asset cache
 * @param sid    String ID of the asset
 * @param type   String ID of the asset type
 * @param handle Raw pointer to the (heap-allocated) asset
 * @returns LinceTrue if asset was successfully added, and LinceFalse otherwise.
*/
LinceBool LinceAssetCacheAdd(LinceAssetCache* cache, LinceSID sid, LinceSID type, void* handle);

/** @brief Load an asset from memory
 * @param sid  String ID for the asset
 * @param args Custom argument passed to load function
 * @returns pointer to loaded asset, or NULL if asset does not exist, has not been registered, or is already loaded.
*/
void* LinceAssetCacheLoad(LinceAssetCache* cache, LinceSID sid, void* args);

/** @brief Unload a cached asset
 * @param cache Asset cache
 * @param sid   String ID of the asset
 * @returns LinceTrue if the asset was succesfully unloaded,
 *          and LinceFalse otherwise.
*/
LinceBool LinceAssetCacheUnload(LinceAssetCache* cache, LinceSID sid);

/** Reloads an existing asset from disk, discarding the previous one
 * @param sid  String ID of the asset
 * @param args Custom extra arguments for load function
 * @returns raw pointer to reloaded asset, or NULL if the asset does not exist or has not been registered.
 * @note If the asset wasn't previously loaded, it simply loads it.
*/
void* LinceAssetCacheReload(LinceAssetCache* cache, LinceSID sid, void* args);

/** @brief Retrieve a cached asset
 * @param sid String ID
 * @returns Raw pointer to asset, or NULL if asset does not exist or has not been registered.
 * @note If an asset is requested but is not currently loaded,
 * it will attempt to load it with no extra arguments (i.e. args = NULL in LinceAssetCacheLoad).
 * If you want to ensure an asset is loaded with specific arguments,
 * call LinceAssetCacheLoad once with the desired arguments, and then use LinceAssetCacheGet afterwards.
*/
void* LinceAssetCacheGet(LinceAssetCache* cache, LinceSID sid);

/** @brief Set the default asset for a type.
 * If a requested asset does not exist, the default asset is returned instead.
 * The asset to be set as default must have been registered already.
 * If the default asset has not yet been loaded, it will be loaded with args = NULL.
 * @param cache          Asset cache
 * @param type           Asset type for which to set a default
 * @param default_asset  Asset to use as default for this type. Must have been registered.
 * @returns LinceTrue if the default asset was succesfully set/loaded, and LinceFalse otherwise.
*/
LinceBool LinceAssetCacheSetDefault(LinceAssetCache* cache, LinceSID type, LinceSID default_asset);

/** @brief Fetch the default asset for an asset type.
 * If a default has not been set, NULL is returned.
 * @param cache Asset cache
 * @param type  Asset type
 * @returns Handle of default asset if successful, NULL otherwise.
 */
void* LinceAssetCacheGetDefault(LinceAssetCache* cache, LinceSID type);

/** @brief Get the full path of a loaded asset.
 * If the asset does not exist, calling string_ok() on the result will return LinceFalse.
 * @param cache Asset cache
 * @param sid String ID of the asset
 * @returns Full path of the asset
 */
string_t LinceAssetCacheGetPath(LinceAssetCache* cache, LinceSID sid);

// void* LinceAssetCacheLoadAsync(LinceAssetCache* cache, LinceSID sid);


#endif /* LINCE_ASSET_MANAGER_H */