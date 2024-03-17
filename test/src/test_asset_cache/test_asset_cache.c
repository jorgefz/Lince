#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"

/* Verifies the asset cache is successfully initialised */
void test_asset_cache_init(void** state){
    (void)state;
    LinceAssetCache assets;
    LinceInitAssetCache(&assets);

    assert_true(LinceIsDir(assets.exe_dir));

    LinceUninitAssetCache(&assets);
}

/* Verifies adding an existing asset folder is successful */
void test_asset_cache_push_folder_real(void** state){
    (void)state;
    LinceAssetCache assets;
    LinceInitAssetCache(&assets);

    int success;
    const char folder[] =  "../../../lince/assets/";
    size_t exe_path_len = strlen(assets.exe_dir);
    success = LinceAssetCachePushFolder(&assets, folder);
    assert_int_equal(success, 1);
    assert_int_equal(assets.asset_folders.size, 1);

    // First part of asset folder is path to executable
    char* result = array_get(&assets.asset_folders, 0);
    assert_memory_equal(
        result,
        assets.exe_dir,
        exe_path_len
    );
    // Second part of asset folder is custom path pushed earlier
    assert_memory_equal(
        result + exe_path_len,
        folder,
        sizeof(folder)
    );

    LinceUninitAssetCache(&assets);
}


/* Verifies adding an non-existent asset folder is unsuccessful */
void test_asset_cache_push_folder_fake(void** state){
    (void)state;
    LinceAssetCache assets;
    LinceInitAssetCache(&assets);

    int success;
    const char folder[] =  "FAKE/";
    success = LinceAssetCachePushFolder(&assets, folder);

    assert_int_equal(success, 0);
    assert_int_equal(assets.asset_folders.size, 0);

    LinceUninitAssetCache(&assets);
}


/* Verifies the path to a real asset can be sought successfully */
void test_asset_cache_fetch_path_real(void** state){
    (void)state;
    LinceAssetCache assets;
    LinceInitAssetCache(&assets);

    const char folder[] =  "../../../lince/assets/";
    int success = LinceAssetCachePushFolder(&assets, folder);
    assert_int_equal(success, 1);

    const char asset[] = "fonts/DroidSans.ttf";
    char* result = LinceAssetCacheFetchPath(&assets, asset);
    assert_non_null(result);
    assert_true(LinceIsFile(result));

    // Ensure path to asset was built successfully
    size_t exe_len = strlen(assets.exe_dir);
    assert_memory_equal(
        result,
        assets.exe_dir,
        exe_len
    );
    assert_memory_equal(
        result + exe_len,
        folder,
        sizeof(folder)-1 // exclude terminator
    );
    assert_memory_equal(
        result + exe_len + sizeof(folder) - 1,
        asset,
        sizeof(asset)
    );

    LinceUninitAssetCache(&assets);
}

/* Verifies the path to a non-exsitent asset is correctly flagged as such */
void test_asset_cache_fetch_path_fake(void** state){
    (void)state;
    LinceAssetCache assets;
    LinceInitAssetCache(&assets);

    const char folder[] =  "../../../lince/assets/";
    int success = LinceAssetCachePushFolder(&assets, folder);
    assert_int_equal(success, 1);

    const char asset[] = "FAKE";
    char* result = LinceAssetCacheFetchPath(&assets, asset);
    assert_null(result);

    LinceUninitAssetCache(&assets);
}

void test_asset_cache_asset_shadowing(void** state){
    (void)state;
    LinceAssetCache assets;
    LinceInitAssetCache(&assets);

    int success;
    const char folder1[] =  "../../../lince/assets/";
    success = LinceAssetCachePushFolder(&assets, folder1);
    assert_int_equal(success, 1);

    const char folder2[] =  "../../../test/assets/";
    success = LinceAssetCachePushFolder(&assets, folder2);
    assert_int_equal(success, 1);

    const char asset[] = "fonts/DroidSans.ttf";
    char* result = LinceAssetCacheFetchPath(&assets, asset);
    assert_non_null(result);
    assert_true(LinceIsFile(result));

    // Ensure folder 2 overshadows folder 1
    size_t exe_len = strlen(assets.exe_dir);
    assert_memory_equal(
        result + exe_len,
        folder2,
        sizeof(folder2)-1 // exclude terminator
    );

    LinceUninitAssetCache(&assets);
}