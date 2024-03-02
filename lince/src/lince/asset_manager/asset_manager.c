#include "lince/asset_manager/asset_manager.h"
#include "lince/utils/fileio.h"

void LinceInitAssetManager(LinceAssetManager* am){
    array_init(&am->asset_dirs, LINCE_PATH_MAX * sizeof(char));
    LinceFetchExeDir(am->exe_dir, LINCE_PATH_MAX);
}

void LinceUninitAssetManager(LinceAssetManager* am){
    array_uninit(&am->asset_dirs);
}

LinceBool LincePushAssetDir(LinceAssetManager* am, const char* dir){
    uint32_t length = strlen(dir);
    uint32_t exedir_length = strlen(am->exe_dir);
    
    LINCE_ASSERT(length < LINCE_PATH_MAX - exedir_length - 1,
        "Asset directory is too long. Length %u but max is %u",
        length, LINCE_PATH_MAX-exedir_length-1);
    
    array_push_front(&am->asset_dirs, NULL);
    char* p = array_front(&am->asset_dirs);

    // Prepend directory of executable
    memmove(p, am->exe_dir, exedir_length);
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

    if(LinceIsDir(array_front(&am->asset_dirs)) != 1){
        LINCE_WARN("Failed to add assets folder because it does not exist: '%s'",
            (char*)array_front(&am->asset_dirs));
        array_pop_front(&am->asset_dirs);
        return LinceFalse;
    }

    LINCE_INFO("Added assets folder '%s'", (char*)array_front(&am->asset_dirs));
    return LinceTrue;
}


char* LinceFetchAssetPath(LinceAssetManager* am, const char* asset_filename){
    for(uint32_t i = 0; i != am->asset_dirs.size; ++i){
        char* dir = array_get(&am->asset_dirs, i);
        uint32_t dir_len = strlen(dir);
        
        if (dir_len + strlen(asset_filename) >= LINCE_PATH_MAX){
            LINCE_WARN("Skipping path, too long: '%s' + '%s'", dir, asset_filename);
            continue;
        }
        
        memmove(am->result_path, dir, dir_len);
        memmove(am->result_path + dir_len, asset_filename, strlen(asset_filename)+1);
        if (LinceIsFile(am->result_path)){
            LINCE_INFO("Located asset '%s' at '%s'", asset_filename, am->result_path);
            return am->result_path;
        }
    }
    LINCE_WARN("Could not locate asset '%s'", asset_filename);
    return NULL;
}
