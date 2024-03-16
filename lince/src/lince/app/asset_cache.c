#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"
#include "lince/core/logger.h"

void LinceInitAssetCache(LinceAssetCache* cache){
    array_init(&cache->asset_dirs, LINCE_PATH_MAX * sizeof(char));
    LinceFetchExeDir(cache->exe_dir, LINCE_PATH_MAX);
}

void LinceUninitAssetCache(LinceAssetCache* cache){
    array_uninit(&cache->asset_dirs);
}

LinceBool LinceAssetCachePushDir(LinceAssetCache* cache, const char* dir){
    uint32_t length = (uint32_t)strlen(dir);
    uint32_t exedir_length = (uint32_t)strlen(cache->exe_dir);
    
    LINCE_ASSERT(length < LINCE_PATH_MAX - exedir_length - 1,
        "Asset directory is too long. Length %u but max is %u",
        length, LINCE_PATH_MAX-exedir_length-1);
    
    array_push_front(&cache->asset_dirs, NULL);
    char* p = array_front(&cache->asset_dirs);

    // Prepend directory of executable
    memmove(p, cache->exe_dir, exedir_length);
    if (p[exedir_length-1] != '\\' && p[exedir_length-1] != '/'){
        p[exedir_length] = '/';
        exedir_length++;
    }
    p += exedir_length;

    // Append relative directory to assets folder
    memmove(p, dir, length);
    if (p[length-1] != '\\' && p[length-1] != '/'){
        p[length] = '/';
        length++;
    }
    p[length] = '\0';

    if(LinceIsDir(array_front(&cache->asset_dirs)) != 1){
        LINCE_WARN("Failed to add assets folder because it does not exist: '%s'",
            (char*)array_front(&cache->asset_dirs));
        array_pop_front(&cache->asset_dirs);
        return LinceFalse;
    }

    LINCE_INFO("Added assets folder '%s'", (char*)array_front(&cache->asset_dirs));
    return LinceTrue;
}


char* LinceAssetCacheFetchPath(LinceAssetCache* cache, const char* asset_filename){
    for(uint32_t i = 0; i != cache->asset_dirs.size; ++i){
        char* dir = array_get(&cache->asset_dirs, i);
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
