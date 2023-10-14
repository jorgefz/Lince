#ifndef LINCE_ASSET_MANAGER_H
#define LINCE_ASSET_MANAGER_H

#include "lince/core/core.h"
#include "lince/containers/array.h"

typedef struct LinceAssetManager {
    char exe_dir[LINCE_PATH_MAX];   ///< Directory where the executable is located
    array_t asset_dirs;             ///< Array of relative paths between executable and asset folders
    char result_path[LINCE_PATH_MAX]; ///< Stores result of LinceFetchAssetPath
} LinceAssetManager;


// Gets directory of executable, initialises array of asset dirs
void LinceInitAssetManager(LinceAssetManager* am);

void LinceUninitAssetManager(LinceAssetManager* am);

/** @brief Adds the location of an asset folder to the list of search paths.
* The path must be relative to the location of the executable.
* @param dir Zero-terminated string with the path to an asset folder to store.
* @returns LinceTrue if the asset folder exists, LinceFalse otherwise.
*/
LinceBool LincePushAssetDir(LinceAssetManager* am, const char* dir);

/** @brief Retrieves the full path of an asset file by searching on the asset folders
* @note the resulting path is written to the `result_path` field in `LinceAssetManager`.
* @param asset_filename Location of the asset file within an asset folder
* @returns a pointer to the `result_path` field with the resolved asset path, or NULL if the asset could not be found
*/
char* LinceFetchAssetPath(LinceAssetManager* am, const char* asset_filename);


#endif /* LINCE_ASSET_MANAGER_H */