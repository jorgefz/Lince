#ifndef LINCE_ASSET_MANAGER_H
#define LINCE_ASSET_MANAGER_H

#include "lince/core/core.h"
#include "lince/containers/array.h"

typedef struct LinceAssetCache {
    char exe_dir[LINCE_PATH_MAX];   ///< Directory where the executable is located
    array_t asset_dirs;             ///< Array of relative paths between executable and asset folders
    char result_path[LINCE_PATH_MAX]; ///< Stores result of LinceFetchAssetPath
} LinceAssetCache;


/** @brief Initialise asset cache */
void LinceInitAssetCache(LinceAssetCache* cache);

/** @brief Uninitialise asset cache, freeing internally allocated memory */
void LinceUninitAssetCache(LinceAssetCache* cache);

/** @brief Saves the location of an asset folder to the list of search paths.
* The path must be relative to the location of the executable.
* @param dir Zero-terminated string with the path to an asset folder to store.
* @returns LinceTrue if the asset folder exists, LinceFalse otherwise.
*/
LinceBool LinceAssetCachePushDir(LinceAssetCache* cache, const char* dir);

/** @brief Retrieves the full path of an asset file by searching in the stored asset folders
* @param filename Location of the asset file within an asset folder
* @returns a pointer to the resulting path, or NULL if the asset could not be found
* @note the path returned is saved internally and will be overwritten the next time the function is called.
*/
char* LinceAssetCacheFetchPath(LinceAssetCache* cache, const char* filename);


#endif /* LINCE_ASSET_MANAGER_H */