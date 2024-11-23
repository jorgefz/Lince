#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"
#include "lince/core/logger.h"
#include "lince/utils/memory.h"


LinceBool LinceInitAssetCache(LinceAssetCache* cache) {
    char exedir_buf[LINCE_PATH_MAX];
    size_t exedir_len = LinceFetchExecutablePath(exedir_buf, LINCE_PATH_MAX);
    if (exedir_len == 0) {
        return LinceFalse;
    }

    cache->exedir = string_from_len(exedir_len + 1);

    cache->exedir = string_from_chars(exedir_buf, exedir_len);
    LINCE_INFO("Located executable at '%s'", cache->exedir.str);

    array_init(&cache->folders, sizeof(string_t));
    hashmap_init(&cache->stores, 10);

    return LinceTrue;
}


void LinceUninitAssetCache(LinceAssetCache* cache) {
    string_free(&cache->exedir);

    for(string_t* s = cache->folders.begin; s != cache->folders.end; ++s){
        string_free(s);
    }
    array_uninit(&cache->folders);

    char* type = NULL;
    uint32_t type_len = 0;
    while ((type = hashmap_iterb(&cache->stores, type, type_len, &type_len))) {

        LinceAssetStore* st = hashmap_getb(&cache->stores, type, type_len);
        
        char* asset = NULL;
        uint32_t asset_len = 0;
        while ((asset = hashmap_iterb(&st->handles, asset, asset_len, &asset_len))) {
            LinceAssetCacheUnload(cache, string_scoped(asset,asset_len), string_scoped(type,type_len));
        }
        hashmap_uninit(&st->handles);
        LinceFree(st);
    
	}
    hashmap_uninit(&cache->stores);
}


LinceAssetCache* LinceCreateAssetCache() {
    LinceAssetCache* cache = LinceMalloc(sizeof(LinceAssetCache));
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
    
    if(LinceIsDir(assets_dir.str) != LinceTrue){
        LINCE_WARN("Failed to add assets folder because it does not exist: '%s'", assets_dir);
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
        
        string_t full_path = string_from_fmt("%s%s", dir->str, filename.str);

        if (LinceIsFile(full_path.str)){
            LINCE_INFO("Located asset '%s' at '%s'", filename.str, full_path.str);
            return full_path;
        }
        string_free(&full_path); // Inefficient to allocate and deallocate every loop
    }
    LINCE_WARN("Could not locate asset '%s'", filename.str);
    return (string_t){0};
}

/** @brief Registers a new type of asset on the asset cache.
 * @param cache Asset cache
 * @param name String identifier of the asset type. Must not be in use.
 * @param load Function to load the asset given a file path.
 * @param unload Function to free the asset from memory.
*/
void* LinceAssetCacheAddType(
        LinceAssetCache* cache,
        string_t         name,
        LinceAssetLoad   load,
        LinceAssetUnload unload
    ){
    if(!cache || !name.str || !load || !unload) return NULL;
    
    if (hashmap_has_keyb(&cache->stores, name.str, name.len)){
        LINCE_WARN("Failed to add asset type '%s' to asset cache because it already exists", name.str);
        return NULL;
    }

    LinceAssetStore* st = LinceMalloc(sizeof(LinceAssetStore));
    LINCE_ASSERT_ALLOC(st, sizeof(LinceAssetCache));
    st->callbacks = (LinceAssetCallbacks){.load = load, .unload = unload};
    hashmap_init(&st->handles, 10);
    hashmap_setb(&cache->stores, name.str, name.len, st);

    return cache;
}

void* LinceAssetCacheAdd(LinceAssetCache* cache, string_t name, string_t type, void* handle){
    if(!name.str || !type.str || !handle) return NULL;

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    if(!st){
        LINCE_WARN("Asset type '%s' does not exist in asset cache", type.str);
        return NULL;
    }

    if(hashmap_getb(&st->handles, name.str, name.len)){
        LINCE_WARN("Failed to add asset '%s' to asset cache because it is already loaded", name);
        return NULL;
    }

    hashmap_setb(&st->handles, name.str, name.len, handle);
    return handle;
}


void* LinceAssetCacheLoad(LinceAssetCache* cache, string_t name, string_t type, void* args){
    if (!cache || !name.str || !type.str) return NULL;
    
    string_t path = LinceAssetCacheFetchPath(cache, name);
    if(!path.str) return NULL;

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);

    if(!st){
        // Create asset store automatically?
        LINCE_WARN("Asset type '%s' does not exist", type);
        string_free(&path);
        return NULL;
    }

    if(hashmap_getb(&st->handles, name.str, name.len)){
        LINCE_WARN("Did not load asset '%s' (%s) because it is already loaded", name.str, type.str);
        string_free(&path);
        return NULL;
    }

    void* handle = st->callbacks.load(path, args);
    hashmap_setb(&st->handles, name.str, name.len, handle);
    string_free(&path);
    return handle;
}

void* LinceAssetCacheUnload(LinceAssetCache* cache, string_t name, string_t type){
    if (!cache || !name.str || !type.str) return NULL;

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    if(!st){
        LINCE_WARN("Asset type '%s' does not exist", type);
        return NULL;
    }

    void* handle = hashmap_getb(&st->handles, name.str, name.len);
    if(!handle) return NULL;

    st->callbacks.unload(handle);
    hashmap_setb(&st->handles, name.str, name.len, NULL);

    return cache;
}

void* LinceAssetCacheReload(LinceAssetCache* cache, string_t name, string_t type, void* args){
    if (!cache || !name.str || !type.str) return NULL;

    LinceAssetCacheUnload(cache, name, type); // Does nothing if asset is not loaded
    return LinceAssetCacheLoad(cache, name, type, args);
}


void* LinceAssetCacheGet(LinceAssetCache* cache, string_t name, string_t type){
    if(!cache || !name.str || !type.str) return NULL;

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    if(!st){
        LINCE_WARN("Asset type '%s' does not exist", type);
        return NULL;
    }
    void* handle = hashmap_getb(&st->handles, name.str, name.len);
    if(handle) return handle;

    return LinceAssetCacheLoad(cache, name, type, NULL);
}

