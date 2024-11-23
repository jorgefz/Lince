#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"
#include "lince/core/logger.h"
#include "lince/utils/memory.h"


LinceBool LinceInitAssetCache(LinceAssetCache* cache) {
    static char exedir_buf[LINCE_PATH_MAX];
    size_t exedir_len = LinceFetchExecutablePath(exedir_buf, LINCE_PATH_MAX);
    if (exedir_len == 0) {
        return LinceFalse;
    }

    cache->exedir = string_from_len(exedir_len + 1);

    cache->exedir = string_from_chars(exedir_buf, exedir_len);
    LINCE_INFO("Located executable at '%s'", cache->exedir.str);

    array_init(&cache->folders, LINCE_PATH_MAX * sizeof(char));
    hashmap_init(&cache->stores, 10);

    return LinceTrue;
}


void LinceUninitAssetCache(LinceAssetCache* cache) {
    string_free(&cache->exedir);
    array_uninit(&cache->folders);

    char* type_key = NULL;
    while ((type_key = hashmap_iter(&cache->stores, type_key))) {

        LinceAssetStore* st = hashmap_get(&cache->stores, type_key);
        
        char* asset_key = NULL;
        while ((asset_key = hashmap_iter(&st->handles, asset_key))) {
            LinceAssetCacheUnload(cache, asset_key, type_key);
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
    
    char* assets_dir = array_push_front(&cache->folders, NULL);

    // Concatenate executable path and asset folder path
    memmove(assets_dir, cache->exedir.str, cache->exedir.len);
    memmove(assets_dir + cache->exedir.len, path.str, path.len);

    size_t total_len = path.len + cache->exedir.len;
    char* end = assets_dir + total_len ;

    // Add slash separator at the end if missing
    if (end[-1] != '\\' && end[-1] != '/'){
        end[0] = '/';
        end[1] = '\0';
    }
    
    if(LinceIsDir(array_front(&cache->folders)) != 1){
        LINCE_WARN("Failed to add assets folder because it does not exist: '%s'", assets_dir);
        array_pop_front(&cache->folders);
        return LinceFalse;
    }

    LINCE_INFO("Added assets folder '%s'", assets_dir);
    return LinceTrue;
}


char* LinceAssetCacheFetchPath(LinceAssetCache* cache, const char* asset_filename){

    array_t* folders = &cache->folders;
    for(uint32_t i = 0; i != folders->size; ++i){
       char* dir = array_get(&cache->folders, i);
        uint32_t dir_len = (uint32_t)strlen(dir);
        if (dir_len + strlen(asset_filename) >= LINCE_PATH_MAX){
            LINCE_WARN("Skipping path, too long: '%s' + '%s'", dir, asset_filename);
            continue;
        }

        LINCE_INFO("Checking asset folder '%s'", dir);
        
        memmove(cache->result_path, dir, dir_len);
        memmove(cache->result_path + dir_len, asset_filename, strlen(asset_filename)+1);
        if (LinceIsFile(cache->result_path)){
            LINCE_INFO("Located asset '%s' at '%s'", asset_filename, cache->result_path);
            return cache->result_path;
        }
    }
    LINCE_WARN("Could not locate asset '%s'", asset_filename);
    return NULL;
}

/** @brief Registers a new type of asset on the asset cache.
 * @param cache Asset cache
 * @param name String identifier of the asset type. Must not be in use.
 * @param load Function to load the asset given a file path.
 * @param unload Function to free the asset from memory.
*/
void* LinceAssetCacheAddAssetType(
        LinceAssetCache* cache,
        const char* name,
        LinceAssetLoad load,
        LinceAssetUnload unload
    ){
    if(!cache || !name || !load || !unload) return NULL;
    
    if (hashmap_has_key(&cache->stores, name)){
        LINCE_WARN("Failed to add asset type '%s' to asset cache because it already exists", name);
        return NULL;
    }

    LinceAssetStore* st = LinceMalloc(sizeof(LinceAssetStore));
    LINCE_ASSERT_ALLOC(st, sizeof(LinceAssetCache));

    st->callbacks = (LinceAssetCallbacks){.load = load, .unload = unload};
    size_t name_size = strlen(name) + 1; // Copy terminator
    memcpy(st->type, name, name_size < LINCE_NAME_MAX ? name_size : LINCE_NAME_MAX);
    hashmap_init(&st->handles, 10); // Default init size
    hashmap_set(&cache->stores, name, st);

    return cache;
}

void* LinceAssetCacheAdd(LinceAssetCache* cache, const char* name, const char* type, void* handle){
    if(!name || !type || !handle) return NULL;

    LinceAssetStore* st = hashmap_get(&cache->stores, type);
    if(!st){
        LINCE_WARN("Asset type '%s' does not exist in asset cache", type);
        return NULL;
    }

    if(hashmap_get(&st->handles, name)){
        LINCE_WARN("Failed to add asset '%s' to asset cache because it is already loaded", name);
        return NULL;
    }

    hashmap_set(&st->handles, name, handle);
    return handle;
}


void* LinceAssetCacheLoad(LinceAssetCache* cache, const char* name, const char* type, void* args){
    if (!cache || !name || !type) return NULL;
    
    char* path = LinceAssetCacheFetchPath(cache, name);
    if(!path) return NULL;

    LinceAssetStore* st = hashmap_get(&cache->stores, type);

    if(!st){
        // Create asset store automatically?
        LINCE_WARN("Asset type '%s' does not exist", type);
        return NULL;
    }

    if(hashmap_get(&st->handles, name)){
        return NULL; // Asset already loaded
    }

    void* handle = st->callbacks.load(path, args);
    hashmap_set(&st->handles, name, handle);
    return handle;
}

void* LinceAssetCacheUnload(LinceAssetCache* cache, const char* name, const char* type){
    if (!cache || !name || !type) return NULL;

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

void* LinceAssetCacheReload(LinceAssetCache* cache, const char* name, const char* type, void* args){
    if (!cache || !name || !type) return NULL;

    LinceAssetCacheUnload(cache, name, type); // Does nothing if asset wasn't loaded
    return LinceAssetCacheLoad(cache, name, type, args);
}


void* LinceAssetCacheGet(LinceAssetCache* cache, const char* name, const char* type){
    if(!cache || !name || !type) return NULL;

    LinceAssetStore* st = hashmap_get(&cache->stores, type);
    if(!st){
        LINCE_WARN("Asset type '%s' does not exist", type);
        return NULL;
    }
    void* handle = hashmap_get(&st->handles, name);
    if(handle) return handle;

    return LinceAssetCacheLoad(cache, name, type, NULL);
}

