#ifndef LINCE_ASSET_MANAGER_H
#define LINCE_ASSET_MANAGER_H

#include "lince/core/core.h"
#include "lince/containers/array.h"
#include "lince/containers/hashmap.h"

enum LinceAssetType {
    LinceAsset_None = -1,
    LinceAsset_Texture = 0,
    // LinceAsset_Image,
    // LinceAsset_TextFile,
    // LinceAsset_ShaderHeader,
    // LinceAsset_ShaderProgram,
    // LinceAsset_Font,
    // LinceAsset_Misc, // Generic files
    LinceAsset_Count // Number of asset types
};

typedef struct LinceAssetCache {
    char exedir[LINCE_PATH_MAX];     ///< Directory where the executable is located
    size_t exedir_length;
    char result_path[LINCE_PATH_MAX]; ///< Stores result of LinceFetchAssetPath
    array_t folders;  ///< Array of relative paths between executable and asset folders
    array_t stores;   ///< array<hashmap> Stores info and handles for each asset type, indexed by LinceAssetType
    
} LinceAssetCache;



/** @brief Initialise asset cache
 * @returns LinceTrue if successful, and LinceFalse otherwise
*/
LinceBool LinceInitAssetCache(LinceAssetCache* cache);

/** @brief Uninitialise asset cache, freeing internally allocated memory */
void LinceUninitAssetCache(LinceAssetCache* cache);

/** @brief Saves the location of an asset folder to the list of search paths.
* The path must be relative to the location of the executable.
* @param dir Zero-terminated string with the path to an asset folder to store.
* @returns LinceTrue if the asset folder exists, LinceFalse otherwise.
*/
LinceBool LinceAssetCachePushFolder(LinceAssetCache* cache, const char* folder);

/** @brief Retrieves the full path of an asset file by searching in the stored asset folders
* @param filename Location of the asset file within an asset folder
* @returns a pointer to the resulting path, or NULL if the asset could not be found
* @note the path returned is saved internally and will be overwritten the next time the function is called.
*/
char* LinceAssetCacheFetchPath(LinceAssetCache* cache, const char* filename);

void LinceAssetCacheLoad(LinceAssetCache* cache, const char* filename, int type);


#endif /* LINCE_ASSET_MANAGER_H */