#include "core/core.h"
#include "core/uuid.h"

#ifdef LINCE_WINDOWS
    #include <rpcdce.h>
#elif defined(LINCE_LINUX)
    #include <uuid/uuid.h>
#endif


LinceUUID LinceNewUUID(){
    LinceUUID uuid;

#ifdef LINCE_WINDOWS
    UUID win_uuid;
    RPC_STATUS err =  UuidCreate(&win_uuid);
    LINCE_ASSERT(
        err == RPC_S_OK || err == RPC_S_UUID_LOCAL_ONLY,
        "Failed to generate UUID"
    );
    memmove(&uuid, &win_uuid, sizeof(LinceUUID));

#elif defined(LINCE_LINUX)
    uuid_t unix_uuid;
    uuid_generate(unix_uuid);
    memmove(&uuid, unix_uuid, sizeof(LinceUUID));

#endif

    return uuid;
}


LinceBool LinceUUIDEquals(LinceUUID uu1, LinceUUID uu2){
    return (uu1.lo==uu2.lo && uu1.hi==uu2.hi);
}


void LinceUUIDToStr(LinceUUID uuid, char* buf, uint64_t size){
    LINCE_ASSERT(buf, "Can't parse UUID to string: NULL buffer");
    LINCE_ASSERT(size >= LINCE_UUID_STR_SIZE,
        "Can't parse UUID to string: buffer must be at least 37 chars in size");
    uint8_t* b = (uint8_t*)&uuid;
    int ret = snprintf(
        buf, size,
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        b[0],b[1],b[2],b[3],  b[4],b[5],  b[6],b[7], b[8],b[9],
        b[10],b[11],b[12],b[13],b[14],b[15]    
    );
    LINCE_ASSERT(ret == LINCE_UUID_STR_SIZE-1, "Failed to format UUID as a string");
}