#ifndef TEST_ASSET_CACHE
#define TEST_ASSET_CACHE

#define ASSET_CACHE_TESTS \
    cmocka_unit_test(test_asset_cache_init), \
    cmocka_unit_test(test_asset_cache_push_folder_real), \
    cmocka_unit_test(test_asset_cache_push_folder_fake), \
    

/* Verifies the asset cache is successfully initialised */
void test_asset_cache_init(void** state);

/* Verifies adding an existing asset folder is successful */
void test_asset_cache_push_folder_real(void** state);

/* Verifies adding an non-existant asset folder is unsuccessful */
void test_asset_cache_push_folder_fake(void** state);

#endif /* TEST_ASSET_CACHE */