#ifndef LINCE_SID
#define LINCE_SID

#include <dast.h>
#include "lince/core/core.h"

/** @typedef Definition for String ID */
typedef uint64_t LinceSID;

/** @def Helper macro to create a SID from a string literal */
#define  LinceSIDFromLit(LIT) LinceMakeSID(string_scoped_lit(LIT))

/** @brief Initialise SID cache that maps SID to original string */
hashmap_t* LinceInitSIDCache();

/** @brief Delete SID cache */
void LinceUninitSIDCache();

/** @brief Create a SID from a string.
 * @param name String to convert to SID
 * @note If SID cache has not been created, the input string is not stored.
*/
LinceSID LinceMakeSID(string_t name);

/** @brief Fetch the string associated with a SID.
 * @param sid SID
 * @returns String associated with the input SID.
 * @note If the input SID does not exist or the SID cache has not been initialised,
 *       this function returns (string_t){0}, and passing the return value to 'string_ok'
 *       will result in 'dast_false'.
 */
string_t LinceGetSIDName(LinceSID sid);

#endif /* LINCE_SID */