#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"
#include "lince/core/logger.h"

LinceBool LinceInitAssetCache(LinceAssetCache* cache){
    cache->exedir_length = LinceFetchExeDir(cache->exedir, LINCE_PATH_MAX);
    if(cache->exedir_length == 0){
        return LinceFalse;
    }

    int ret = array_init(&cache->folders, LINCE_PATH_MAX * sizeof(char));
    if(ret != 1){
        return LinceFalse;
    }

    return LinceTrue;
}

void LinceUninitAssetCache(LinceAssetCache* cache){
    array_uninit(&cache->folders);
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

/*
void LinceAssetCacheLoad(LinceAssetCache* cache, const char* filename, int type){

    // Build full path
    // path = cache->exedir + asset_folder + filename

    void* handle;
    char* path;

    switch(type){
        case LinceAsset_Texture:
            handle = LinceLoadTexture(path, 0);
            break;
        case LinceAsset_Shader:
            handle = LinceLoadShader(path, headers);
        case LinceAsset_Font:
            handle = LinceUILoadFonts(path, sizes);
    }

}
*/