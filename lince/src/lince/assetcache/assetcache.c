#include "lince/assetcache/assetcache.h"
#include "lince/utils/fileio.h"


LinceBool LinceInitAssetCache(LinceAssetCache* cache) {

    string_t buf = string_from_len(LINCE_PATH_MAX);
    
    size_t exedir_len = LinceFetchExecutablePath(buf.str, buf.len);
    if (exedir_len == 0) {
        string_free(&buf);
        return LinceFalse;
    }

    cache->exedir = string_from_chars(buf.str, exedir_len);
    LINCE_INFO("Located executable at '%s'", cache->exedir.str);

    string_free(&buf);
    array_init_custom(&cache->folders, sizeof(string_t), LINCE_DAST_ARRAY_ALLOCATOR);
    hashmap_init_custom(&cache->assets, 10, LINCE_DAST_HASHMAP_ALLOCATOR, NULL, LinceSIDCmp);
    hashmap_init_custom(&cache->types, 10, LINCE_DAST_HASHMAP_ALLOCATOR, NULL, LinceSIDCmp);

    return LinceTrue;
}


void LinceUninitAssetCache(LinceAssetCache* cache) {

    string_free(&cache->exedir);

    for(string_t* s = cache->folders.begin; s != cache->folders.end; ++s){
        string_free(s);
    }
    array_uninit(&cache->folders);

    // Unload and delete assets
    LinceSID* psid = NULL;
    uint64_t sz = sizeof(LinceSID);
    while((psid=hashmap_iterb(&cache->assets, psid, &sz))){
        LinceAssetCacheUnload(cache, *psid);
        LinceAsset* asset = hashmap_getb(&cache->assets, psid, sizeof(LinceSID));
        string_free(&asset->path);
        LinceFree(asset);
    }
    hashmap_uninit(&cache->assets);

    // Free loaders
    psid = NULL;
    while((psid=hashmap_iterb(&cache->types, psid, &sz))){
        LinceAssetLoader* loader = hashmap_getb(&cache->types, psid, sizeof(LinceSID));
        LinceFree(loader);
    }
    hashmap_uninit(&cache->types);
}


LinceAssetCache* LinceCreateAssetCache() {
    LinceAssetCache* cache = LinceAlloc(sizeof(LinceAssetCache));
    if (!cache) return NULL;
    if (!LinceInitAssetCache(cache)) {
        LinceFree(cache);
        return NULL;
    }
    return cache;
}


void LinceDeleteAssetCache(LinceAssetCache* cache) {
    LinceUninitAssetCache(cache);
    LinceFree(cache);
}


LinceBool LinceAssetCachePushFolder(LinceAssetCache* cache, string_t path){

    if(path.len + cache->exedir.len + 1 >= LINCE_PATH_MAX){ // Extra space for terminating char
        LINCE_WARN("Failed to add assets folder because its full path is longer than %ld", LINCE_PATH_MAX);
        return LinceFalse;
    }

    // Extra space at the end of fmt to allow for extra slash separator if needed
    string_t assets_dir = string_from_fmt("%s%s ", cache->exedir.str, path.str);

    // Add slash separator at the end if missing
    char* end = assets_dir.str + assets_dir.len - 1;
    if (end[-1] != '\\' && end[-1] != '/'){
        end[0] = '/';
        end[1] = '\0';
    } else {
        end[0] = '\0';
    }

    if(!LinceIsDir(assets_dir)){
        LINCE_WARN("Failed to add assets folder because it does not exist: '%s'", assets_dir.str);
        string_free(&assets_dir);
        return LinceFalse;
    }

    array_push_front(&cache->folders, &assets_dir);

    LINCE_INFO("Added assets folder '%s'", assets_dir.str);
    return LinceTrue;
}


/** @brief Retrieves the full path of an asset file by searching in the stored asset folders
* @param filename Location of the asset file within an asset folder
* @returns the full path of the asset; which needs to be freed with `string_free`.
*/
string_t LinceAssetCacheFetchPath(LinceAssetCache* cache, string_t filename){

    array_t* folders = &cache->folders;

    for(string_t* dir = folders->begin; dir != folders->end; ++dir){
        LINCE_INFO("Searching for asset '%s' in assets folder '%s'", filename.str, dir->str);
        
        if (dir->len + filename.len >= LINCE_PATH_MAX){
            LINCE_WARN("Skipping path, too long: '%s' + '%s'", dir->str, filename.str);
            continue;
        }
        
        string_t full_path = string_from_fmt_custom(LINCE_DAST_STRING_ALLOCATOR, "%s%s", dir->str, filename.str);

        if (LinceIsFile(full_path)){
            LINCE_INFO("Located asset '%s' at '%s'", filename.str, full_path.str);
            return full_path;
        }
        string_free(&full_path); // Inefficient to allocate and deallocate every loop
    }
    LINCE_WARN("Could not locate asset '%s'", filename.str);
    return (string_t){0};
}

/** @brief Registers a new type of asset on the asset cache.
 * An asset type defines an interface to load and unload assets of the same kind.
 * @param cache  Asset cache
 * @param name   String ID of the asset type. Must not be in use.
 * @param load   Function to load the asset given a file path.
 * @param unload Function to free the asset from memory.
 * @note If type already exists, it's loader is updated to the input functions.
 * @returns LinceTrue on success and LinceFalse otherwise.
*/
LinceBool LinceAssetCacheAddType(
        LinceAssetCache* cache,
        LinceSID         type_sid,
        LinceAssetLoad   load,
        LinceAssetUnload unload
    ){
    if(!cache || !load || !unload) return LinceFalse;

    LinceAssetLoader* loader;
    loader = hashmap_getb(&cache->types, &type_sid, sizeof(LinceSID));
    
    if (loader){
        loader->load = load;
        loader->unload = unload;
        LINCE_WARN("Asset type '%s' already exists. Loader updated.", LinceGetSIDName(type_sid).str);
        return LinceTrue;
    }

    LinceAssetLoader new_loader = {.load = load, .unload = unload};
    loader = LinceNewCopy(&new_loader, sizeof(LinceAssetLoader));
    hashmap_setb(&cache->types, &type_sid, sizeof(LinceSID), loader);

    LINCE_INFO("Added new asset type '%s'", LinceGetSIDName(type_sid).str);
    return LinceTrue;
}

/** @brief Registers an asset to the cache. Stores its type and filename, but does not load it.
 * @param cache Asset cache
 * @param sid   String ID of the asset
 * @param type  String ID of its type
 * @param path  Location of the asset inside an asset folder.
 * @returns LinceTrue if asset was succesfully registered, and LinceFalse if the asset
 *          cannot be located on disk or if it has already been registered.
*/
LinceBool LinceAssetCacheRegister(LinceAssetCache* cache, LinceSID sid, LinceSID type, string_t path){
    if(!cache || !string_ok(path)) return LinceFalse;

    if(hashmap_has_keyb(&cache->assets, &sid, sizeof(LinceSID))){
        LINCE_WARN("Asset '%s' has already been registered", LinceGetSIDName(sid).str);
        return LinceFalse;
    }

    string_t full_path = LinceAssetCacheFetchPath(cache, path);
    if(!string_ok(full_path)) {
        return LinceFalse;
    }

    LinceAsset* asset_data = LinceCalloc(sizeof(LinceAsset));
    asset_data->sid = sid;
    asset_data->type = type;
    asset_data->path = full_path;
    hashmap_setb(&cache->assets, &sid, sizeof(LinceSID), asset_data);

    LINCE_INFO("Registered new asset '%s' located at '%s'", LinceGetSIDName(sid).str, full_path.str);
    return LinceTrue;
}

/** @brief Adds a pre-loaded asset to the cache.
 * The asset must be heap-allocated and not registered.
 * Passing it to the cache will mean transfering ownership to it, so don't free it yourself!
 * @param cache  Asset cache
 * @param sid    String ID of the asset
 * @param type   String ID of the asset type
 * @param handle Raw pointer to the (heap-allocated) asset data
 * @returns LinceTrue if asset was successfully added, and LinceFalse otherwise.
*/
LinceBool LinceAssetCacheAdd(LinceAssetCache* cache, LinceSID sid, LinceSID type, void* handle){
    if(!cache|| !handle) return LinceFalse;

    if(!hashmap_has_keyb(&cache->types, &type, sizeof(LinceSID))){
        LINCE_WARN("Could not add asset '%s' as it has an invalid type", LinceGetSIDName(sid).str);
        LinceFalse;
    }

    if(hashmap_has_keyb(&cache->assets, &sid, sizeof(LinceSID))){
        // Asset already registered/loaded
        LINCE_WARN("Could not add asset '%s' as it is already registered", LinceGetSIDName(sid).str);
        return LinceFalse;
    }

    LinceAsset* asset_data = LinceCalloc(sizeof(LinceAsset));
    asset_data->sid = sid;
    asset_data->type = type;
    // asset_data->path = string_from_literal_custom("...", LINCE_DAST_STRING_ALLOCATOR);
    hashmap_setb(&cache->assets, &sid, sizeof(LinceSID), asset_data);

    return LinceTrue;
}

/** @brief Load an asset from memory.
 * @param sid  String ID for the asset
 * @param args Custom argument passed to load function
 * @returns pointer to loaded asset, or NULL if the asset does not exist,
 *          has not been registered, or is already loaded.
*/
void* LinceAssetCacheLoad(LinceAssetCache* cache, LinceSID sid, void* args){
    if (!cache) return NULL;
    
    LinceAsset* asset = hashmap_getb(&cache->assets, &sid, sizeof(LinceSID));
    if(!asset || !string_ok(asset->path)){
        LINCE_WARN("Asset '%s' does not exist", LinceGetSIDName(sid).str);
        return NULL;
    } else if (asset->handle){
        LINCE_WARN("Asset '%s' already loaded", LinceGetSIDName(sid).str);
        return NULL;
    }

    LinceAssetLoader* loader = hashmap_getb(&cache->types, &asset->type, sizeof(LinceSID));
    if(!loader){
        LINCE_WARN("Asset '%s' has an invalid loader", LinceGetSIDName(sid).str);
        return NULL;
    }

    asset->handle = loader->load(asset->path, args);
    //hashmap_setb(&cache->assets, &sid, sizeof(LinceSID), handle);
    return asset->handle;
}

/** @brief Unload a cached asset
 * @param cache Asset cache
 * @param sid   String ID of the asset
 * @returns LinceTrue if the asset was succesfully unloaded,
 *          and LinceFalse if the asset does not exist.
*/
LinceBool LinceAssetCacheUnload(LinceAssetCache* cache, LinceSID sid){
    if (!cache) return LinceFalse;

    LinceAsset* asset = hashmap_getb(&cache->assets, &sid, sizeof(LinceSID));
    if(!asset || !asset->handle){
        LINCE_WARN("Asset '%s' does not exist or is not loaded", LinceGetSIDName(sid).str);
        return LinceFalse;
    }

    LinceAssetLoader* loader = hashmap_getb(&cache->types, &asset->type, sizeof(LinceSID));
    if (!loader) return LinceFalse;

    loader->unload(asset->handle);
    asset->handle = NULL;

    return LinceTrue;
}

/** Reloads an existing asset from disk, discarding the previous one.
 * If the asset was added instead of being loaded, it will unload it but it won't be able to load it back.
 * @param sid  String ID of the asset
 * @param args Custom extra arguments for load function
 * @returns raw pointer to reloaded asset, or NULL if the asset does not exist or has not been registered.
 * @note If the asset wasn't previously loaded, it simply loads it.
*/
void* LinceAssetCacheReload(LinceAssetCache* cache, LinceSID sid, void* args){
    if (!cache) return NULL;
    LinceAssetCacheUnload(cache, sid); // Does nothing if asset is not loaded
    return LinceAssetCacheLoad(cache, sid, args);
}

/** @brief Retrieve a cached asset
 * @param sid String ID
 * @returns Raw pointer to asset, or NULL if asset does not exist or has not been registered.
 * @note If an asset is requested but is not currently loaded,
 * it will attempt to load it with no extra arguments (i.e. args = NULL in LinceAssetCacheLoad).
 * If you want to ensure an asset is loaded with specific arguments,
 * call LinceAssetCacheLoad once with the desired arguments, and then use LinceAssetCacheGet afterwards.
*/
void* LinceAssetCacheGet(LinceAssetCache* cache, LinceSID sid){
    if(!cache) return NULL;

    LinceAsset* asset = hashmap_getb(&cache->assets, &sid, sizeof(LinceSID));
    if(!asset){
        LINCE_WARN("Asset '%s' does not exist or has not been registered", LinceGetSIDName(sid).str);
        return NULL;
    }
    if(asset->handle) return asset->handle;
    return LinceAssetCacheLoad(cache, sid, NULL);
}

