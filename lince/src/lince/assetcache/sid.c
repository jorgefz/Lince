#include "sid.h"

/** @todo Make thread-safe when I integrate CThreads */
static hashmap_t SID_CACHE = {0};

dast_bool LinceSIDCmp(const void* a, const void* b, dast_sz len){
    (void)len;
    return *(LinceSID*)a == *(LinceSID*)b;
}

hashmap_t* LinceInitSIDCache(){
    if (SID_CACHE.table){
        return &SID_CACHE;
    }
    return hashmap_init_custom(&SID_CACHE, 10, LINCE_DAST_HASHMAP_ALLOCATOR, NULL, LinceSIDCmp);
}

void LinceUninitSIDCache(){
    if(!SID_CACHE.table) return;
    LinceSID* sid = NULL;
    uint64_t sz = sizeof(LinceSID);
    do {
        sid = (LinceSID*)hashmap_iterb(&SID_CACHE, (void*)sid, &sz);
        string_t *s = hashmap_getb(&SID_CACHE, sid, sizeof(LinceSID));
        string_free(s);
        LinceFree(s);
    } while(sid);
    hashmap_uninit(&SID_CACHE);
    SID_CACHE = (hashmap_t){0};
}

/* Input string is copied into a global table */
LinceSID LinceMakeSID(string_t name){
	LinceSID sid = (LinceSID)hashmap_FNV1a64_hash(name.str, name.len);
    if(SID_CACHE.table){
        string_t* s = LinceAlloc(sizeof(string_t));
        *s = string_copy_custom(name, LINCE_DAST_STRING_ALLOCATOR);
        hashmap_setb(&SID_CACHE, &sid, sizeof(LinceSID), s);
    }
	return sid;
}

/* Return value is scoped - do not free or use after scope ends */
string_t LinceGetSIDName(LinceSID sid){
    if (!SID_CACHE.table) return (string_t){0};
    string_t* s = hashmap_getb(&SID_CACHE, &sid, sizeof(LinceSID));
    if(!s) return (string_t){0};
    return *s;
}

/** @brief Return a pointer to the SID cache */
hashmap_t* LinceGetSIDCache(){
    return &SID_CACHE;
}