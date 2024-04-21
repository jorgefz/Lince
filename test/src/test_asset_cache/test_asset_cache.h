#ifndef TEST_ASSET_CACHE
#define TEST_ASSET_CACHE

#define TEST_GROUP_ASSET_CACHE \
    cmocka_unit_test(test_asset_cache_init), \
    cmocka_unit_test(test_asset_cache_push_folder_real), \
    cmocka_unit_test(test_asset_cache_push_folder_fake), \
    cmocka_unit_test(test_asset_cache_fetch_path_real), \
    cmocka_unit_test(test_asset_cache_fetch_path_fake), \
    cmocka_unit_test(test_asset_cache_asset_shadowing), \
    cmocka_unit_test(test_asset_cache_add_asset_type),  \
    cmocka_unit_test(test_asset_cache_add), \
    cmocka_unit_test(test_asset_cache_load)
    


/* Verifies the asset cache is successfully initialised */
void test_asset_cache_init(void** state);

/* Verifies adding an existing asset folder is successful */
void test_asset_cache_push_folder_real(void** state);

/* Verifies adding an non-existant asset folder is unsuccessful */
void test_asset_cache_push_folder_fake(void** state);

/* Verifies the path to a real asset can be sought successfully */
void test_asset_cache_fetch_path_real(void** state);

/* Verifies the path to a non-existent asset is correctly flagged as such */
void test_asset_cache_fetch_path_fake(void** state);

/* Verifies any existing engine asset can be shadowed by another asset named identically */
void test_asset_cache_asset_shadowing(void** state);

/* Verify a new asset type can be added to the asset cache */
void test_asset_cache_add_asset_type(void** state);

/* Adds an asset to an existing cache */
void test_asset_cache_add(void** state);

/* Verifies an asset is loaded from disk */
void test_asset_cache_load(void** state);

#endif /* TEST_ASSET_CACHE */