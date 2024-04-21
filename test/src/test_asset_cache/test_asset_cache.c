#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/app/asset_cache.h"
#include "lince/utils/fileio.h"
#include "lince/utils/image.h"
#include "lince/utils/memory.h"


static void* mock_load_asset(const char* path, void* args){
    (void) args;
    assert_non_null(path);
    int* asset = malloc(sizeof(int));
    *asset = 100;
    return asset;
}

static void mock_unload_asset(void* obj){
    assert_non_null(obj);
    int* asset = obj;
    assert_int_equal(*asset, 100);
    free(asset);
}

static void* mock_load_image(const char* path, void* args){
    (void) args;
    assert_true(LinceIsFile(path));
    
    LinceImage* img = LinceMalloc(sizeof(LinceImage));
    void* ret = LinceLoadImage(img, path);
    assert_non_null(ret);

    return img;
}

static void mock_unload_image(void* img){
    LinceDeleteImage(img);
    LinceFree(img);
}



/* Verifies the asset cache is successfully initialised */
void test_asset_cache_init(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    assert_true(LinceIsDir(cache->exedir));
    assert_int_equal(cache->folders.size, 0);
    assert_non_null(cache->stores.table);

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


void test_asset_cache_add_asset_type(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    void* success = LinceAssetCacheAddAssetType(cache,
        "mock_asset_type", mock_load_asset, mock_unload_asset);

    assert_non_null(success);
    assert_true(hashmap_has_key(&cache->stores, "mock_asset_type"));
    LinceAssetStore* store = hashmap_get(&cache->stores, "mock_asset_type");
    assert_non_null(store);

    LinceDeleteAssetCache(cache);
}


/* Adds an asset to an existing cache */
void test_asset_cache_add(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCacheAddAssetType(cache,
        "mock_asset_type", mock_load_asset, mock_unload_asset);
    int* asset = mock_load_asset("mock_path", NULL);

    void* success = LinceAssetCacheAdd(cache, "asset", "mock_asset_type", asset);
    
    assert_ptr_equal(success, asset);
    LinceAssetStore* st = hashmap_get(&cache->stores, "mock_asset_type");
    assert_non_null(st);
    assert_true(hashmap_has_key(&st->handles, "asset"));
    assert_ptr_equal(hashmap_get(&st->handles, "asset"), asset);
    
    LinceDeleteAssetCache(cache);
}

void test_asset_cache_load(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, "../../../test/assets/");
    LinceAssetCacheAddAssetType(
        cache, "image", mock_load_image, mock_unload_image
    );

    LinceImage* img = LinceAssetCacheLoad(cache, "images/test.png", "image", NULL);
    assert_non_null(img);

    LinceAssetStore* st = hashmap_get(&cache->stores, "image");
    assert_true(hashmap_has_key(&st->handles, "images/test.png"));
    assert_ptr_equal(hashmap_get(&st->handles, "images/test.png"), img);
    assert_int_equal(img->width, 48);
    assert_int_equal(img->height, 16);

    LinceDeleteAssetCache(cache);
}