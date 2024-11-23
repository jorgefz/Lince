
#ifndef LINCE_FILEIO_H
#define LINCE_FILEIO_H

#include "lince/core/core.h"
#include "lince/containers/array.h"
#include "lince/containers/str.h"

/** @brief Returns the directory of the running executable, with the filename stripped,
 * and ending in a slash (forward or backward).
 * If the length of the requested path exceeds that of the buffer, zero is returned. 
 * The output string will be null-terminated.
 * @param buf Buffer where the directory of executable is copied to.
 * @param max_size Maximum number of bytes to copy to the buffer.
 * @returns Number of bytes copied to the input buffer, excluding the terminating character
 */
size_t LinceFetchExecutablePath(char* buf, size_t max_size);

/** @brief Returns LinceTrue if a given file exists
 * @param path Path to file
 */
LinceBool LinceIsFile(const char* path);

/** @brief Returns LinceTrue if a given path exists
 * @param path Directory path
 */
LinceBool LinceIsDir(const char* path);

/** @brief Loads a file's contents into memory.
 * On fail, the returning string will have the field `str` set to NULL and its size set to zero.
 * @note Returned string must be freed with `string_free`.
 * @param path path to file to load
 * @returns file content
 */
string_t LinceReadFile(string_t path);


#endif /* LINCE_FILEIO_H */