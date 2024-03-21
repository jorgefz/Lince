
#ifndef LINCE_FILEIO_H
#define LINCE_FILEIO_H

#include "lince/core/core.h"
#include "lince/containers/array.h"

/** @brief Returns the directory of the running executable with the filename stripped.
* If the length of the requested path exceeds that of the buffer, zero is returned. 
* The output string will be null-terminated.
* @param buf Buffer where the directory of executable is copied to.
* @param max_size Maximum number of bytes to copy to the buffer.
* @returns Number of bytes copied to the input buffer, excluding the terminating character
*/
size_t LinceFetchExeDir(char* buf, size_t max_size);

/** @brief Returns LinceTrue if a given file exists
* @param path Path to file
*/
LinceBool LinceIsFile(const char* path);

/** @brief Returns LinceTrue if a given path exists
* @param path Directory path
*/
LinceBool LinceIsDir(const char* path);


/** @brief Copies a binary file's contents into memory.
* @note Returned memory must be freed.
* @param filepath path to file to load
*/
char* LinceLoadFile(const char* path);

/** @brief Copies a text file's contents into memory, appending a terminator at the end.
* @note Returned memory must be freed.
* @param filepath path to file to load
*/
char* LinceLoadTextFile(const char* path);

#endif /* LINCE_FILEIO_H */