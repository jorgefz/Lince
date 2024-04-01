#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"
#include "lince/core/logger.h"
#include "lince/utils/memory.h"

#include "lince/utils/image.h"
#include "lince/renderer/texture.h"


/* ============== STATIC VARIABLES ============== */




/* ============== STATIC FUNCTION DECLARATIONS ============== */

/** @brief Initialise asset cache
 * The user is forced to use `LinceCreateAssetCache` because LinceAssetCache is large
 * and it's a bad idea to keep it in the stack.
 * @returns LinceTrue if successful, and LinceFalse otherwise
 */
static LinceBool LinceInitAssetCache(LinceAssetCache* cache);

/** @brief Uninitialise asset cache, freeing internally allocated memory */
static void LinceUninitAssetCache(LinceAssetCache* cache);

static void* LinceAssetCacheLoadImage(const char* path, void* args);
static void LinceAssetCacheUnloadImage(void* handle);

static void* LinceAssetCacheLoadTexture(const char* path, void* args);
static void LinceAssetCacheUnloadTexture(void* handle);


static const LinceAssetCallbacks _asset_callbacks[LinceAssetType_Count] = {
        [LinceAssetType_Image] = {LinceAssetCacheLoadImage, LinceAssetCacheUnloadImage},
        [LinceAssetType_Texture] = {LinceAssetCacheLoadTexture, LinceAssetCacheUnloadTexture}
};



/* ============== STATIC FUNCTION DEFINITIONS ============== */


static LinceBool LinceInitAssetCache(LinceAssetCache* cache) {
    cache->exedir_length = LinceFetchExeDir(cache->exedir, LINCE_PATH_MAX);
    if (cache->exedir_length == 0) {
        return LinceFalse;
    }

    array_init(&cache->folders, LINCE_PATH_MAX * sizeof(char));

    // Initialise stores
    array_init(&cache->stores, sizeof(LinceAssetStore));
    array_resize(&cache->stores, LinceAssetType_Count);
    for (uint32_t i = 0; i != LinceAssetType_Count; ++i) {
        LinceAssetStore* st = array_get(&cache->stores, i);
        st->type = (int)i;
        st->callbacks = _asset_callbacks[i];
        hashmap_init(&st->handles, 10);
    }

    return LinceTrue;
}


static void LinceUninitAssetCache(LinceAssetCache* cache) {
    array_uninit(&cache->folders);
    for (int i = 0; i != LinceAssetType_Count; ++i) {
        LinceAssetStore* st = array_get(&cache->stores, i);
        char* key = NULL;
        while ((key = hashmap_iter(&st->handles, key))) {
            LinceAssetCacheUnload(cache, key, i);
        }
        hashmap_uninit(&st->handles);
    }
    array_uninit(&cache->stores);
}


static void* LinceAssetCacheLoadImage(const char* path, void* args) {
    LinceImage* handle = LinceMalloc(sizeof(LinceImage));
    void* ret = LinceLoadImage(handle, path);
    if (!ret) {
        LinceFree(handle);
        return NULL;
    }
    return handle;
}

static void LinceAssetCacheUnloadImage(void* handle) {
    LinceDeleteImage(handle);
    LinceFree(handle);
}

static void* LinceAssetCacheLoadTexture(const char* path, void* args) {
    return LinceCreateTextureFromFile(path, 0);
}

static void LinceAssetCacheUnloadTexture(void* handle) {
    LinceDeleteTexture(handle);
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

void* LinceAssetCacheAdd(LinceAssetCache* cache, const char* name, int type, void* handle){
    if(!handle) return NULL;
    if (type < 0 || type >= LinceAssetType_Count) return NULL;
    
    LinceAssetStore* st = array_get(&cache->stores, type);
    hashmap_set(&st->handles, name, handle);
    return handle;
}


void* LinceAssetCacheGet(LinceAssetCache* cache, const char* name, int type){
    if(type < 0 || type >= LinceAssetType_Count) return NULL;

    LinceAssetStore* st = array_get(&cache->stores, type);
    hashmap_t* handles = &st->handles;
    void* handle = hashmap_get(handles, name);
    if(handle) return handle;

    return LinceAssetCacheLoad(cache, name, type, NULL);
}


void* LinceAssetCacheLoad(LinceAssetCache* cache, const char* name, int type, void* args){
    if (type < 0 || type >= LinceAssetType_Count) return NULL;
    
    char* path = LinceAssetCacheFetchPath(cache, name);
    if(!path) return NULL;

    LinceAssetStore* st = array_get(&cache->stores, type);
    if(hashmap_get(&st->handles, name)){
        return NULL; // Asset already loaded
    }

    void* handle = st->callbacks.load(path, args);
    hashmap_set(&st->handles, name, handle);
    return handle;
}

void LinceAssetCacheUnload(LinceAssetCache* cache, const char* name, int type){
    if (type < 0 || type >= LinceAssetType_Count) return;

    LinceAssetStore* st = array_get(&cache->stores, type);
    void* handle = hashmap_get(&st->handles, name);
    if(!handle) return;

    st->callbacks.unload(handle);
    hashmap_set(&st->handles, name, NULL);
}

void* LinceAssetCacheReload(LinceAssetCache* cache, const char* name, int type, void* args){
    if(type < 0 || type >= LinceAssetType_Count){
        return NULL;
    }
    LinceAssetCacheUnload(cache, name, type); // Does nothing if asset wasn't loaded
    return LinceAssetCacheLoad(cache, name, type, args);
}
