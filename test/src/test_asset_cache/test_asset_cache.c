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


static void* mock_load_asset(string_t path, void* args){
    (void) args;
    assert_non_null(path.str);
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

static void* mock_load_image(string_t path, void* args){
    (void) args;
    assert_true(LinceIsFile(path.str));
    
    LinceImage* img = LinceLoadImage(path.str);
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
    string_t path =  string_scoped_lit("../../../lince/assets/");
    size_t exe_path_len = cache->exedir.len;

    success = LinceAssetCachePushFolder(cache, path);
    assert_int_equal(success, 1);
    assert_int_equal(cache->folders.size, 1);

    // First part of asset folder is path to executable
    string_t* result = array_get(&cache->folders, 0);
    assert_memory_equal(
        result->str,
        cache->exedir.str,
        exe_path_len
    );
    // Second part of asset folder is custom path pushed earlier
    assert_memory_equal(
        result->str + exe_path_len,
        path.str,
        path.len
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

    string_t path = string_scoped_lit("../../../lince/assets/");
    string_t name = string_scoped_lit("fonts/DroidSans.ttf");
    
    int success = LinceAssetCachePushFolder(cache, path);
    assert_int_equal(success, 1);

    string_t result = LinceAssetCacheFetchPath(cache, name);
    assert_non_null(result.str);
    assert_true(LinceIsFile(result.str));

    // Ensure path to asset was built successfully
    size_t exe_len = cache->exedir.len;
    assert_memory_equal(
        result.str,
        cache->exedir.str,
        exe_len
    );
    assert_memory_equal(result.str + exe_len, path.str, path.len);
    assert_memory_equal(result.str + exe_len + path.len, name.str, name.len);

    string_free(&result);
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
    string_t result = LinceAssetCacheFetchPath(cache, string_scoped_lit(asset));
    assert_null(result.str);

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
    string_t result = LinceAssetCacheFetchPath(cache, string_scoped_lit(asset));
    assert_non_null(result.str);
    assert_true(LinceIsFile(result.str));

    // Ensure folder 2 overshadows folder 1
    size_t exe_len = cache->exedir.len;
    assert_memory_equal(
        result.str + exe_len,
        folder2,
        sizeof(folder2)-1 // exclude terminator
    );

    string_free(&result);
    LinceDeleteAssetCache(cache);
}

void test_asset_cache_add_asset_type(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();
    string_t type = string_scoped_lit("mock_asset_type");

    void* success = LinceAssetCacheAddType(cache, type, mock_load_asset, mock_unload_asset);

    assert_non_null(success);
    assert_true(hashmap_has_keyb(&cache->stores, type.str, type.len));
    LinceAssetStore* store = hashmap_getb(&cache->stores,type.str, type.len);
    assert_non_null(store);

    LinceDeleteAssetCache(cache);
}

void test_asset_cache_add_existing_asset_type(void** state){
    (void)state;
    LinceAssetCache* cache = LinceCreateAssetCache();
    const char type[] = "mock_asset_type";

    void* success = LinceAssetCacheAddType(
        cache, string_scoped_lit(type),
        mock_load_asset, mock_unload_asset
    );
    assert_non_null(success);

    success = LinceAssetCacheAddType(
        cache, string_scoped_lit(type),
        mock_load_asset, mock_unload_asset
    );
    assert_null(success);

    LinceDeleteAssetCache(cache);
}


/* Adds an asset to an existing cache */
void test_asset_cache_add(void** state){
    (void)state;

    string_t type  = string_scoped_lit("mock_asset_type");
    string_t name = string_scoped_lit("asset");
    string_t path  = string_scoped_lit("mock_path");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCacheAddType(cache, type, mock_load_asset, mock_unload_asset);
    int* asset_data = mock_load_asset(path, NULL);

    void* success = LinceAssetCacheAdd(cache, name, type, asset_data);
    
    assert_ptr_equal(success, asset_data);
    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    assert_non_null(st);
    assert_true(hashmap_has_keyb(&st->handles, name.str, name.len));
    assert_ptr_equal(hashmap_getb(&st->handles, name.str, name.len), asset_data);
    
    LinceDeleteAssetCache(cache);
}


void test_asset_cache_add_existing(void** state){
    (void)state;
    
    string_t type  = string_scoped_lit("mock_asset_type");
    string_t name = string_scoped_lit("asset");
    string_t path  = string_scoped_lit("mock_path");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCacheAddType(cache, type, mock_load_asset, mock_unload_asset);
    int* asset1 = mock_load_asset(path, NULL);
    int* asset2 = mock_load_asset(path, NULL);

    void* success1 = LinceAssetCacheAdd(cache, name, type, asset1);
    void* success2 = LinceAssetCacheAdd(cache, name, type, asset2);
    
    assert_non_null(success1);
    assert_null(success2);

    mock_unload_asset(asset2);
    LinceDeleteAssetCache(cache);
}


void test_asset_cache_add_with_invalid_type(void** state){
    (void)state;

    string_t type  = string_scoped_lit("invalid_asset_type");
    string_t name = string_scoped_lit("asset");
    string_t path  = string_scoped_lit("mock_path");

    LinceAssetCache* cache = LinceCreateAssetCache();
    int* asset = mock_load_asset(path, NULL);

    void* success = LinceAssetCacheAdd(cache, name, type, asset);
    assert_null(success);
    
    mock_unload_asset(asset);
    LinceDeleteAssetCache(cache);
}


void test_asset_cache_load(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    string_t name = string_scoped_lit("images/test.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    LinceImage* img = LinceAssetCacheLoad(cache, name, type, NULL);
    assert_non_null(img);

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    assert_true(hashmap_has_keyb(&st->handles, name.str, name.len));
    assert_ptr_equal(hashmap_getb(&st->handles, name.str, name.len), img);
    assert_int_equal(img->width, 48);
    assert_int_equal(img->height, 16);

    LinceDeleteAssetCache(cache);
}


void test_asset_cache_load_invalid_path(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    string_t name_fake = string_scoped_lit("fake/fake.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    LinceImage* img = LinceAssetCacheLoad(cache, name_fake, type, NULL);
    assert_null(img);

    LinceDeleteAssetCache(cache);
}


/* Verifies an asset is unloaded */
void test_asset_cache_unload(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    string_t name = string_scoped_lit("images/test.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);
    LinceAssetCacheLoad(cache, name, type, NULL);
    LinceAssetCacheUnload(cache, name, type);

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    assert_true(hashmap_has_keyb(&st->handles, name.str, name.len));
    assert_null(hashmap_getb(&st->handles, name.str, name.len));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is not unloaded when it does not exist */
void test_asset_cache_unload_invalid(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    string_t name = string_scoped_lit("images/test.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    void* success = LinceAssetCacheUnload(cache, name, type);
    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    
    assert_null(success);
    assert_false(hashmap_has_keyb(&st->handles, name.str, name.len));
    assert_null(hashmap_getb(&st->handles, name.str, name.len));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is reloaded */
void test_asset_cache_reload(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    string_t name = string_scoped_lit("images/test.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheLoad(cache, name, type, NULL);
    assert_non_null(img);

    void* img2 = LinceAssetCacheReload(cache, name, type, NULL);
    assert_non_null(img2);
    // Now `img` points to invalid memory!

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    assert_true(hashmap_has_keyb(&st->handles, name.str, name.len));
    assert_non_null(hashmap_getb(&st->handles, name.str, name.len));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is reloaded even when it has not yet been loaded */
void test_asset_cache_reload_invalid(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    string_t name = string_scoped_lit("images/test.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheReload(cache, name, type, NULL);
    assert_non_null(img);

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    assert_true(hashmap_has_keyb(&st->handles, name.str, name.len));
    assert_non_null(hashmap_getb(&st->handles, name.str, name.len));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is reloaded even when it has been unloaded */
void test_asset_cache_reload_unloaded(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    string_t name = string_scoped_lit("images/test.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    LinceAssetCacheLoad(cache, name, type, NULL);
    LinceAssetCacheUnload(cache, name, type);

    void* img2 = LinceAssetCacheReload(cache, name, type, NULL);
    assert_non_null(img2);

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    assert_true(hashmap_has_keyb(&st->handles, name.str, name.len));
    assert_non_null(hashmap_getb(&st->handles, name.str, name.len));

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset can be retrieved */
void test_asset_cache_get(void** state){
    (void)state;
    
    string_t type  = string_scoped_lit("image");
    string_t name = string_scoped_lit("images/test.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheLoad(cache, name, type, NULL);

    void* img2 = LinceAssetCacheGet(cache, name, type);
    assert_non_null(img2);
    assert_ptr_equal(img, img2);

    LinceDeleteAssetCache(cache);
}

/* Verifies an asset can be retrieved */
void test_asset_cache_get_and_load(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    string_t name = string_scoped_lit("images/test.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheGet(cache, name, type);
    assert_non_null(img);

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    assert_true(hashmap_has_keyb(&st->handles, name.str, name.len));
    assert_non_null(hashmap_getb(&st->handles, name.str, name.len));
    
    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is not retrieved when it does not exist */
void test_asset_cache_get_invalid(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    // string_t name = string_scoped_lit("images/test.png");
    string_t name_invalid = string_scoped_lit("images/fake.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    void* img = LinceAssetCacheGet(cache, name_invalid, type);
    assert_null(img);

    LinceAssetStore* st = hashmap_getb(&cache->stores, type.str, type.len);
    assert_false(hashmap_has_keyb(&st->handles, name_invalid.str, name_invalid.len));
    assert_null(hashmap_getb(&st->handles, name_invalid.str, name_invalid.len));
    
    LinceDeleteAssetCache(cache);
}

/* Verifies an asset is loaded and retrieved when it has been unloaded */
void test_asset_cache_get_unloaded(void** state){
    (void)state;

    string_t type  = string_scoped_lit("image");
    string_t name = string_scoped_lit("images/test.png");
    string_t path  = string_scoped_lit("../../../test/assets/");

    LinceAssetCache* cache = LinceCreateAssetCache();
    LinceAssetCachePushFolder(cache, path);
    LinceAssetCacheAddType(cache, type, mock_load_image, mock_unload_image);

    LinceAssetCacheLoad(cache, name, type, NULL);
    LinceAssetCacheUnload(cache, name, type);

    void* img = LinceAssetCacheGet(cache, name, type);
    assert_non_null(img);
    
    LinceDeleteAssetCache(cache);
}
