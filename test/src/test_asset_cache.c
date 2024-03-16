#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"

/* Verifies the asset cache is successfully initialised */
void test_asset_cache_init(){
    LinceAssetCache assets;
    LinceInitAssetCache(&assets);

    assert_true(LinceIsFile(&assets.exe_dir));

    LinceUninitAssetCache(&assets);
}

/* Verifies adding an existing asset folder is successful */
void test_asset_cache_push_folder_real(){
    LinceAssetCache assets;
    LinceInitAssetCache(&assets);

    int success;
    const char folder[] =  "../../../lince/assets";
    size_t exe_path_len = strlen(assets.exe_dir);
    success = LinceAssetCachePushFolder(&assets, folder);

    assert_int_equal(success, 1);
    assert_int_equal(assets.asset_folders.size, 1);
    // First part of asset folder is path to executable
    assert_memory_equal(
        assets.asset_folders.data,
        assets.exe_dir,
        exe_path_len
    );
    // Second part of asset folder is custom path pushed earlier
    assert_memory_equal(
        (char*)assets.asset_folders.data + exe_path_len,
        folder,
        sizeof(folder)
    );

    LinceUninitAssetCache(&assets);
}