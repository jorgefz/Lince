
#ifndef LINCE_FILEIO_H
#define LINCE_FILEIO_H

#include "lince/core/core.h"
#include "lince/containers/array.h"

/** @brief Finds the directory (with the filename stripped) of the running executable 
* @param exe_path Directory of executable is written to this location
* @param max_size Size of the provided buffer
* @note the output string will be null-terminated.
*/
void LinceFetchExeDir(char* exe_path, uint32_t max_size);

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