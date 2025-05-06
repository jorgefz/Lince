#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "dast.h"
#include "hashmap.h"

#include <windows.h>


#define NKEYS 100000
#define KEYLEN 30

void test_hashmap_collisions(void){

    // Generate keys
    srand((unsigned int)time(NULL));
    static char keys[NKEYS][KEYLEN + 1];
    for(uint32_t i = 0; i != NKEYS; ++i){
        for(uint32_t j = 0; j != KEYLEN; ++j){
            keys[i][j] = (char)(  rand() % ('z' + 1 - 'a') + 'a' );
        }
        keys[i][KEYLEN] = '\0';
    }
    printf("Keys generated\n");

    hashmap_t map;
    void* p = hashmap_init(&map, NKEYS*10);
    printf("Map initialised\n");

    // Insert keys
    for(uint32_t i = 0; i != NKEYS; ++i){
        hashmap_setb(&map, keys[i], KEYLEN+1, NULL);
    }
    printf("Keys inserted\n");

    // Scan for collisions
    hashmap_entry_t* entry;
    uint32_t collisions = 0;
    for(uint64_t i = 0; i != map.size; ++i){
        entry = map.table[i];
        if(entry && entry->next){
            printf("Collision of %s with ", entry->key);
            entry = entry->next;
            while (entry) {
                printf(" %s, ", entry->key);
                entry = entry->next;
            }
            printf("\n");
            collisions++;
        }
    }

    // Stats
    dast_sz mapsz = map.size;
    double loadf = (double)NKEYS / (double)mapsz;
    printf("Keys %u and map size %u (loading factor %.2g)\n", NKEYS, (unsigned int)map.size, loadf);
    printf("Collisions: %u\n", collisions);

    hashmap_uninit(&map);

    // Hashing test
    dast_u64 h1 = hashmap_FNV1a64_hash("danhuugadk", KEYLEN+1) % mapsz;
    dast_u64 h2 = hashmap_FNV1a64_hash("ympzximqqj", KEYLEN+1) % mapsz;
    printf("%llu = %llu (%d)\n", h1, h2, h1==h2);
}

dast_bool _key_u64_cmp(const void*a, const void* b, dast_sz len){
    return *(dast_u64*)a == *(dast_u64*)b;
}

void test_string_vs_int_keys(){

    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    double interval;

    QueryPerformanceFrequency(&frequency);
    srand((unsigned int)time(NULL));

    //// String keys
    // Generate keys
    printf("STR Generating keys\n");
    static char keys[NKEYS][KEYLEN + 1];
    for(uint32_t i = 0; i != NKEYS; ++i){
        for(uint32_t j = 0; j != KEYLEN; ++j){
            keys[i][j] = (char)(  rand() % ('z' + 1 - 'a') + 'a' );
        }
        keys[i][KEYLEN] = '\0';
    }
    
    hashmap_t a;
    hashmap_init(&a, NKEYS*2);

    // Insert keys
    printf("STR Inserting keys\n");
    for(uint32_t i = 0; i != NKEYS; ++i){
        hashmap_setb(&a, keys[i], KEYLEN+1, NULL);
    }
    
    // Fetch values
    printf("STR Fetching keys\n");

    QueryPerformanceCounter(&start);
    for(uint32_t i = 0; i != NKEYS; ++i){
        hashmap_getb(&a, keys[i], KEYLEN+1);
    }
    QueryPerformanceCounter(&end);

    hashmap_uninit(&a);

    interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("STR Keys: %f s\n", interval);

    //// Integer keys
    // Generate keys
    printf("INT Generating keys\n");
    static uint64_t ikeys[NKEYS];
    for(dast_sz i = 0; i != NKEYS; ++i){
        ikeys[i] = (uint64_t)rand();
    }

    hashmap_t b;
    hashmap_init_custom(&b, NKEYS*2, (dast_allocator_t){0}, NULL, _key_u64_cmp);

    // Insert keys
    printf("INT Inserting keys\n");
    for(uint32_t i = 0; i != NKEYS; ++i){
        hashmap_setb(&b, &ikeys[i], sizeof(uint64_t), NULL);
    }
    
    // Fetch values
    printf("INT Fetching keys\n");
    
    QueryPerformanceCounter(&start);
    for(uint32_t i = 0; i != NKEYS; ++i){
        hashmap_getb(&b, &ikeys[i], sizeof(uint64_t));
    }
    QueryPerformanceCounter(&end);

    hashmap_uninit(&b);
    
    interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("INT Keys: %f s\n", interval);
}


int main(){
    //test_hashmap_collisions();
    test_string_vs_int_keys();
    return 0;
}