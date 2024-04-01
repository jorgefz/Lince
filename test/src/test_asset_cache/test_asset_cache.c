#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"
#include "lince/utils/image.h"
#include "lince/utils/memory.h"

/* Verifies the asset cache is successfully initialised */
void test_asset_cache_init(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    assert_true(LinceIsDir(cache->exedir));
    
    for (int i = 0; i != LinceAssetType_Count; ++i) {
        LinceAssetStore* st =  array_get(&cache->stores, i);
        assert_int_equal(st->type, i);
        assert_non_null(st->handles.table);
        assert_non_null(st->callbacks.load);
        assert_non_null(st->callbacks.unload);
    }

    LinceDeleteAssetCache(cache);
}

/* Verifies adding an existing asset folder is successful */
void test_asset_cache_push_folder_real(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    LinceBool success;
    const char folder[] =  "../../../lince/assets/";
    size_t exe_path_len = strlen(cache->exedir);
    success = LinceAssetCachePushFolder(cache, folder);
    assert_int_equal(success, 1);
    assert_int_equal(cache->folders.size, 1);

    // First part of asset folder is path to executable
    char* result = array_get(&cache->folders, 0);
    assert_memory_equal(
        result,
        cache->exedir,
        exe_path_len
    );
    // Second part of asset folder is custom path pushed earlier
    assert_memory_equal(
        result + exe_path_len,
        folder,
        sizeof(folder)
    );

    LinceDeleteAssetCache(cache);
}


/* Verifies adding an non-existent asset folder is unsuccessful */
void test_asset_cache_push_folder_fake(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    int success;
    const char folder[] =  "FAKE/";
    success = LinceAssetCachePushFolder(cache, folder);

    assert_int_equal(success, 0);
    assert_int_equal(cache->folders.size, 0);

    LinceDeleteAssetCache(cache);
}


/* Verifies the path to a real asset can be sought successfully */
void test_asset_cache_fetch_path_real(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    const char folder[] =  "../../../lince/assets/";
    int success = LinceAssetCachePushFolder(cache, folder);
    assert_int_equal(success, 1);

    const char asset[] = "fonts/DroidSans.ttf";
    char* result = LinceAssetCacheFetchPath(cache, asset);
    assert_non_null(result);
    assert_true(LinceIsFile(result));

    // Ensure path to asset was built successfully
    size_t exe_len = strlen(cache->exedir);
    assert_memory_equal(
        result,
        cache->exedir,
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

    LinceDeleteAssetCache(cache);
}

/* Verifies the path to a non-exsitent asset is correctly flagged as such */
void test_asset_cache_fetch_path_fake(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    const char folder[] =  "../../../lince/assets/";
    int success = LinceAssetCachePushFolder(cache, folder);
    assert_int_equal(success, 1);

    const char asset[] = "FAKE";
    char* result = LinceAssetCacheFetchPath(cache, asset);
    assert_null(result);

    LinceDeleteAssetCache(cache);
}

void test_asset_cache_asset_shadowing(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    int success;
    const char folder1[] =  "../../../lince/assets/";
    success = LinceAssetCachePushFolder(cache, folder1);
    assert_int_equal(success, 1);

    const char folder2[] =  "../../../test/assets/";
    success = LinceAssetCachePushFolder(cache, folder2);
    assert_int_equal(success, 1);

    const char asset[] = "fonts/DroidSans.ttf";
    char* result = LinceAssetCacheFetchPath(cache, asset);
    assert_non_null(result);
    assert_true(LinceIsFile(result));

    // Ensure folder 2 overshadows folder 1
    size_t exe_len = strlen(cache->exedir);
    assert_memory_equal(
        result + exe_len,
        folder2,
        sizeof(folder2)-1 // exclude terminator
    );

    LinceDeleteAssetCache(cache);
}


/* Adds an asset to an existing cache */
void test_asset_cache_add(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    LinceImage* img = LinceMalloc(sizeof(LinceImage));
    uint8_t img_data[] = {0x1, 0x2, 0x3, 0x4};
    img->height = 1;
    img->width  = 1;
    img->channels = 4;
    img->data = LinceNewCopy(img_data, sizeof(img_data));  ;

    void* retval = LinceAssetCacheAdd(cache, "image", LinceAssetType_Image, img);
    
    assert_ptr_equal(retval, img);
    LinceAssetStore* st = array_get(&cache->stores, LinceAssetType_Image);
    assert_int_equal(hashmap_has_key(&st->handles, "image"), 1);
    assert_ptr_equal(hashmap_get(&st->handles, "image"), img);
    
    LinceDeleteAssetCache(cache);
}