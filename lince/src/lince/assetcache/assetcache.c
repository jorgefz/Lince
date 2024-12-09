#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"
#include "lince/core/logger.h"
#include "lince/utils/memory.h"


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

    string_t type = (string_t){0};
    while ((type = hashmap_iter(&cache->stores, type)).str) {
        LinceAssetStore* st = hashmap_get(&cache->stores, type);
        
        string_t asset = (string_t){0};
        while ((asset = hashmap_iter(&st->handles, asset)).str) {
            LinceAssetCacheUnload(cache, asset, type);
        }
        hashmap_uninit(&st->handles);
        LinceFree(st);
    
	}
    hashmap_uninit(&cache->stores);
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
        
        string_t full_path = string_from_fmt("%s%s", dir->str, filename.str);

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

    LinceAssetStore* st = LinceAlloc(sizeof(LinceAssetStore));
    LINCE_ASSERT_ALLOC(st, sizeof(LinceAssetCache));
    st->callbacks = (LinceAssetCallbacks){.load = load, .unload = unload};
    hashmap_init(&st->handles, 10);
    hashmap_set(&cache->stores, name, st);

    return cache;
}

void* LinceAssetCacheAdd(LinceAssetCache* cache, string_t name, string_t type, void* handle){
    if(!name.str || !type.str || !handle) return NULL;

    LinceAssetStore* st = hashmap_get(&cache->stores, type);
    if(!st){
        LINCE_WARN("Asset type '%s' does not exist in asset cache", type.str);
        return NULL;
    }

    if(hashmap_get(&st->handles, name)){
        LINCE_WARN("Failed to add asset '%s' to asset cache because it is already loaded", name);
        return NULL;
    }

    hashmap_set(&st->handles, name, handle);
    return handle;
}


void* LinceAssetCacheLoad(LinceAssetCache* cache, string_t name, string_t type, void* args){
    if (!cache || !name.str || !type.str) return NULL;
    
    string_t path = LinceAssetCacheFetchPath(cache, name);
    if(!path.str) return NULL;

    LinceAssetStore* st = hashmap_get(&cache->stores, type);

    if(!st){
        // Create asset store automatically?
        LINCE_WARN("Asset type '%s' does not exist", type);
        string_free(&path);
        return NULL;
    }

    if(hashmap_get(&st->handles, name)){
        LINCE_WARN("Did not load asset '%s' (%s) because it is already loaded", name.str, type.str);
        string_free(&path);
        return NULL;
    }

    void* handle = st->callbacks.load(path, args);
    hashmap_set(&st->handles, name, handle);
    string_free(&path);
    return handle;
}

void* LinceAssetCacheUnload(LinceAssetCache* cache, string_t name, string_t type){
    if (!cache || !name.str || !type.str) return NULL;

    LinceAssetStore* st = hashmap_get(&cache->stores, type);
    if(!st){
        LINCE_WARN("Asset type '%s' does not exist", type);
        return NULL;
    }

    void* handle = hashmap_get(&st->handles, name);
    if(!handle) return NULL;

    st->callbacks.unload(handle);
    hashmap_set(&st->handles, name, NULL);

    return cache;
}

void* LinceAssetCacheReload(LinceAssetCache* cache, string_t name, string_t type, void* args){
    if (!cache || !name.str || !type.str) return NULL;

    LinceAssetCacheUnload(cache, name, type); // Does nothing if asset is not loaded
    return LinceAssetCacheLoad(cache, name, type, args);
}


void* LinceAssetCacheGet(LinceAssetCache* cache, string_t name, string_t type){
    if(!cache || !name.str || !type.str) return NULL;

    LinceAssetStore* st = hashmap_get(&cache->stores, type);
    if(!st){
        LINCE_WARN("Asset type '%s' does not exist", type);
        return NULL;
    }
    void* handle = hashmap_get(&st->handles,name);
    if(handle) return handle;

    return LinceAssetCacheLoad(cache, name, type, NULL);
}

