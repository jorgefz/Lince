#include <lince/platform/linux/linux_core.h>
#include <lince/utils/uuid.h>

LinceUUID LinceNewUUID(){
    LinceUUID uuid;
    uuid_t unix_uuid;
    uuid_generate(unix_uuid);
    memmove(&uuid, unix_uuid, sizeof(LinceUUID));
    return uuid;
}