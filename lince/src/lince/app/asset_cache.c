#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"
#include "lince/core/logger.h"
#include "lince/utils/memory.h"

/* ============== STATIC FUNCTION DECLARATIONS ============== */

/** @brief Initialise asset cache
 * The user is forced to use `LinceCreateAssetCache` because LinceAssetCache is large
 * and it's a bad idea to keep it in the stack.
 * @returns LinceTrue if successful, and LinceFalse otherwise
 */
static LinceBool LinceInitAssetCache(LinceAssetCache* cache);

/** @brief Uninitialise asset cache, freeing internally allocated memory */
static void LinceUninitAssetCache(LinceAssetCache* cache);


/* ============== STATIC FUNCTION DEFINITIONS ============== */


static LinceBool LinceInitAssetCache(LinceAssetCache* cache) {
    cache->exedir_length = LinceFetchExeDir(cache->exedir, LINCE_PATH_MAX);
    if (cache->exedir_length == 0) {
        return LinceFalse;
    }

    array_init(&cache->folders, LINCE_PATH_MAX * sizeof(char));
    hashmap_init(&cache->stores, 10);

    return LinceTrue;
}


static void LinceUninitAssetCache(LinceAssetCache* cache) {
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


/* ============== PUBLIC FUNCTION DEFINITIONS ============== */

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


LinceBool LinceAssetCachePushFolder(LinceAssetCache* cache, const char* folder){
    size_t folder_len = strlen(folder);
    size_t exe_len = cache->exedir_length + 1;
    size_t total_len = folder_len + exe_len;
    
    if(total_len >= LINCE_PATH_MAX - 1){
        LINCE_WARN("Failed to add assets folder because its path is too long");
        return LinceFalse;
    }
    
    array_push_front(&cache->folders, NULL);
    char* p = array_front(&cache->folders);

    // Prepend directory of executable
    memmove(p, cache->exedir, exe_len);
    if (p[exe_len-1] != '\\' && p[exe_len-1] != '/'){
        p[exe_len] = '/';
        exe_len++;
    }
    p += exe_len;

    // Append relative directory to assets folder
    memmove(p, folder, folder_len);
    if (p[folder_len-1] != '\\' && p[folder_len-1] != '/'){
        p[folder_len] = '/';
        folder_len++;
    }
    p[folder_len] = '\0';

    if(LinceIsDir(array_front(&cache->folders)) != 1){
        LINCE_WARN("Failed to add assets folder because it does not exist: '%s'",
            (char*)array_front(&cache->folders));
        array_pop_front(&cache->folders);
        return LinceFalse;
    }

    LINCE_INFO("Added assets folder '%s'", (char*)array_front(&cache->folders));
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
    
    if (hashmap_has_key(&cache->stores, name)){
        LINCE_WARN("Failed to add asset type '%s' to asset cache because it already exists", name);
        return NULL;
    }

    LinceAssetStore* st = LinceMalloc(sizeof(LinceAssetStore));
    LINCE_ASSERT_ALLOC(st, sizeof(LinceAssetCache));

    st->callbacks = (LinceAssetCallbacks){.load = load, .unload = unload};
    size_t name_size = strlen(name) + 1; // Copy terminator
    memcpy(st->type, name, name_size < LINCE_NAME_MAX ? name_size : LINCE_NAME_MAX);
    hashmap_init(&st->handles, 10);

    hashmap_set(&cache->stores, name, st);

    return NULL;
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

void LinceAssetCacheUnload(LinceAssetCache* cache, const char* name, const char* type){
    if (!cache || !name || !type) return;

    LinceAssetStore* st = hashmap_get(&cache->stores, type);
    if(!st){
        LINCE_WARN("Asset type '%s' does not exist", type);
        return;
    }

    void* handle = hashmap_get(&st->handles, name);
    if(!handle) return;

    st->callbacks.unload(handle);
    hashmap_set(&st->handles, name, NULL);
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

