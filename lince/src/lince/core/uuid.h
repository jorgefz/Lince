#ifndef LINCE_UUID_H
#define LINCE_UUID_H

#include "lince/core/core.h"

/// Number of characters needed to store an UUID as a string
#define LINCE_UUID_STR_SIZE 37

/** @typedef Holds an UUID (Universal Unique Identifier) */
typedef struct LinceUUID {
    uint64_t hi; ///< Low 64 bits of UUID
    uint64_t lo; ///< High 64 bits of UUID
} LinceUUID;

/** @brief Generates an UUID using platform-specific libraries.
* @return UUID generated
*/
LinceUUID LinceNewUUID();

/** @brief Compares two UUIDs. Returns true if they are equal, and false otherwise.
* @param uu1 First UUID.
* @param uu2 Second UUID.
* @return LinceTrue if UUIDs are equal, and LinceFalse otherwise.
*/
LinceBool LinceUUIDEquals(LinceUUID uu1, LinceUUID uu2);


/** @brief Convert UUID to string representation.
* @param uuid UUID to convert to string.
* @param buf Output buffer pointing to valid memory.
* @param size Size in bytes of the output buffer.
* NOTE: user must provide a buffer with a size of at least 37 characters.
*/
void LinceUUIDToStr(LinceUUID uuid, char* buf, uint64_t size);

#endif /* LINCE_UUID_H */