#include "lince/utils/uuid.h"
#include "lince/core/logger.h"


LinceBool LinceUUIDEquals(LinceUUID uu1, LinceUUID uu2){
    return (uu1.lo==uu2.lo && uu1.hi==uu2.hi);
}


string_t LinceUUIDToStr(LinceUUID uuid){
    uint8_t* b = (uint8_t*)&uuid;
    string_t str = string_from_fmt(
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        b[0],b[1],b[2],b[3],  b[4],b[5],  b[6],b[7], b[8],b[9],
        b[10],b[11],b[12],b[13],b[14],b[15]    
    );
    return str;
}


LinceUUID LinceUUIDFromStr(string_t str){
    LINCE_ASSERT(str.str && str.len == 36,
        "Invalid string UUID - must be 36 chars in length (canonical representation)");

    LinceUUID uuid;
    uint8_t* b = (uint8_t*)&uuid;
    char buf[3] = {0}; // Holds each hex value plus terminator char
    char *p = str.str;

    while(p[0] != '\0' && p[1] != '\0'){
        if(*p == '-'){
            p++;
            continue;
        }
        buf[0] = p[0];
        buf[1] = p[1];
        *b = (uint8_t)strtol(buf, NULL, 16);
        p += 2;
        b++;
    }
    return uuid;
}