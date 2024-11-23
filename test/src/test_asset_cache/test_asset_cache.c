#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "lince/containers/str.h"
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
    
    LinceImage* img = LinceLoadImage(path);
    assert_non_null(img);

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

    assert_true(LinceIsDir(cache->exedir.str));
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
    size_t exe_path_len = cache->exedir.len;
    success = LinceAssetCachePushFolder(cache, string_scoped_lit(folder));
    assert_int_equal(success, 1);
    assert_int_equal(cache->folders.size, 1);

    // First part of asset folder is path to executable
    char* result = array_get(&cache->folders, 0);
    assert_memory_equal(
        result,
        cache->exedir.str,
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
    success = LinceAssetCachePushFolder(cache, string_scoped_lit(folder));

    assert_int_equal(success, 0);
    assert_int_equal(cache->folders.size, 0);

    LinceDeleteAssetCache(cache);
}


/* Verifies the path to a real asset can be sought successfully */
void test_asset_cache_fetch_path_real(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    const char folder[] =  "../../../lince/assets/";
    int success = LinceAssetCachePushFolder(cache, string_scoped_lit(folder));
    assert_int_equal(success, 1);

    const char asset[] = "fonts/DroidSans.ttf";
    char* result = LinceAssetCacheFetchPath(cache, asset);
    assert_non_null(result);
    assert_true(LinceIsFile(result));

    // Ensure path to asset was built successfully
    size_t exe_len = cache->exedir.len;
    assert_memory_equal(
        result,
        cache->exedir.str,
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
    int success = LinceAssetCachePushFolder(cache, string_scoped_lit(folder));
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
    success = LinceAssetCachePushFolder(cache, string_scoped_lit(folder1));
    assert_int_equal(success, 1);

    const char folder2[] =  "../../../test/assets/";
    success = LinceAssetCachePushFolder(cache, string_scoped_lit(folder2));
    assert_int_equal(success, 1);

    const char asset[] = "fonts/DroidSans.ttf";
    char* result = LinceAssetCacheFetchPath(cache, asset);
    assert_non_null(result);
    assert_true(LinceIsFile(result));

    // Ensure folder 2 overshadows folder 1
    size_t exe_len = cache->exedir.len;
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

void test_asset_cache_add_existing_asset_type(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();

    void* success = LinceAssetCacheAddAssetType(
        cache, "mock_asset_type",
        mock_load_asset, mock_unload_asset
    );
    assert_non_null(success);

    success = LinceAssetCacheAddAssetType(
        cache, "mock_asset_type",
        mock_load_asset, mock_unload_asset
    );
    assert_null(success);

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


void test_asset_cache_add_existing(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCacheAddAssetType(cache,
        "mock_asset_type", mock_load_asset, mock_unload_asset);
    int* asset1 = mock_load_asset("mock_path", NULL);
    int* asset2 = mock_load_asset("mock_path", NULL);

    void* success1 = LinceAssetCacheAdd(cache, "asset", "mock_asset_type", asset1);
    void* success2 = LinceAssetCacheAdd(cache, "asset", "mock_asset_type", asset2);
    
    assert_non_null(success1);
    assert_null(success2);

    mock_unload_asset(asset2);
    LinceDeleteAssetCache(cache);
}


void test_asset_cache_add_with_invalid_type(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();
    int* asset = mock_load_asset("mock_path", NULL);

    void* success = LinceAssetCacheAdd(cache, "asset", "invalid_asset_type", asset);
    assert_null(success);
    
    mock_unload_asset(asset);
    LinceDeleteAssetCache(cache);
}


void test_asset_cache_load(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    LinceImage* img = LinceAssetCacheLoad(cache, "images/test.png", "image", NULL);
    assert_non_null(img);

    LinceAssetStore* st = hashmap_get(&cache->stores, "image");
    assert_true(hashmap_has_key(&st->handles, "images/test.png"));
    assert_ptr_equal(hashmap_get(&st->handles, "images/test.png"), img);
    assert_int_equal(img->width, 48);
    assert_int_equal(img->height, 16);

    LinceDeleteAssetCache(cache);
}


void test_asset_cache_load_invalid_path(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    LinceImage* img = LinceAssetCacheLoad(cache, "fake/fake.png", "image", NULL);
    assert_null(img);

    LinceDeleteAssetCache(cache);
}


/* Verifies an asset is unloaded */
void test_asset_cache_unload(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);
    LinceAssetCacheLoad(cache, "images/test.png", "image", NULL);
    LinceAssetCacheUnload(cache, "images/test.png", "image");

    LinceAssetStore* st = hashmap_get(&cache->stores, "image");
    assert_true(hashmap_has_key(&st->handles, "images/test.png"));
    assert_null(hashmap_get(&st->handles, "images/test.png"));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is not unloaded when it does not exist */
void test_asset_cache_unload_invalid(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    void* success = LinceAssetCacheUnload(cache, "images/test.png", "image");

    assert_null(success);
    LinceAssetStore* st = hashmap_get(&cache->stores, "image");
    assert_false(hashmap_has_key(&st->handles, "images/test.png"));
    assert_null(hashmap_get(&st->handles, "images/test.png"));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is reloaded */
void test_asset_cache_reload(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheLoad(cache, "images/test.png", "image", NULL);
    assert_non_null(img);

    void* img2 = LinceAssetCacheReload(cache, "images/test.png", "image", NULL);
    assert_non_null(img2);
    // Now `img` points to invalid memory!

    LinceAssetStore* st = hashmap_get(&cache->stores, "image");
    assert_true(hashmap_has_key(&st->handles, "images/test.png"));
    assert_non_null(hashmap_get(&st->handles, "images/test.png"));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is reloaded even when it has not yet been loaded */
void test_asset_cache_reload_invalid(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheReload(cache, "images/test.png", "image", NULL);
    assert_non_null(img);

    LinceAssetStore* st = hashmap_get(&cache->stores, "image");
    assert_true(hashmap_has_key(&st->handles, "images/test.png"));
    assert_non_null(hashmap_get(&st->handles, "images/test.png"));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is reloaded even when it has been unloaded */
void test_asset_cache_reload_unloaded(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    LinceAssetCacheLoad(cache, "images/test.png", "image", NULL);
    LinceAssetCacheUnload(cache, "images/test.png", "image");

    void* img2 = LinceAssetCacheReload(cache, "images/test.png", "image", NULL);
    assert_non_null(img2);

    LinceAssetStore* st = hashmap_get(&cache->stores, "image");
    assert_true(hashmap_has_key(&st->handles, "images/test.png"));
    assert_non_null(hashmap_get(&st->handles, "images/test.png"));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset can be retrieved */
void test_asset_cache_get(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheLoad(cache, "images/test.png", "image", NULL);

    void* img2 = LinceAssetCacheGet(cache, "images/test.png", "image");
    assert_non_null(img2);
    assert_ptr_equal(img, img2);

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset can be retrieved */
void test_asset_cache_get_and_load(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheGet(cache, "images/test.png", "image");
    assert_non_null(img);

    LinceAssetStore* st = hashmap_get(&cache->stores, "image");
    assert_true(hashmap_has_key(&st->handles, "images/test.png"));
    assert_non_null(hashmap_get(&st->handles, "images/test.png"));
    
    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is not retrieved when it does not exist */
void test_asset_cache_get_invalid(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheGet(cache, "images/fake.png", "image");
    assert_null(img);

    LinceAssetStore* st = hashmap_get(&cache->stores, "image");
    assert_false(hashmap_has_key(&st->handles, "images/fake.png"));
    assert_null(hashmap_get(&st->handles, "images/fake.png"));
    
    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is loaded and retrieved when it has been unloaded */
void test_asset_cache_get_unloaded(void** state){
    (void)state;

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, string_scoped_lit("../../../test/assets/"));
    LinceAssetCacheAddAssetType(cache, "image", mock_load_image, mock_unload_image);

    LinceAssetCacheLoad(cache, "images/test.png", "image", NULL);
    LinceAssetCacheUnload(cache, "images/test.png", "image");

    void* img = LinceAssetCacheGet(cache, "images/test.png", "image");
    assert_non_null(img);
    
    LinceDeleteAssetCache(cache);
}
