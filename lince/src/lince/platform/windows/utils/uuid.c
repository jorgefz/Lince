#include <lince/platform/windows/windows_core.h>
#include <lince/utils/uuid.h>

LinceUUID LinceNewUUID(){
    LinceUUID uuid;
    UUID win_uuid;
    RPC_STATUS err =  UuidCreate(&win_uuid);
    LINCE_ASSERT(
        err == RPC_S_OK || err == RPC_S_UUID_LOCAL_ONLY,
        "Failed to generate UUID"
    );
    memmove(&uuid, &win_uuid, sizeof(LinceUUID));
    return uuid;
}