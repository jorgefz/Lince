#include "lince/core/core.h"
#include "lince/core/logger.h"
#include "lince/containers/array.h"
#include "lince/utils/memory.h"
#include "lince/utils/fileio.h"

#ifdef LINCE_WINDOWS
	#include <sys/types.h>
	#include <sys/stat.h>
	#include "windows.h"
#elif defined(LINCE_LINUX)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif


size_t LinceFetchExeDir(char* buf, size_t max_size){

	size_t n_bytes;
	memset(buf, 0, max_size);

#ifdef LINCE_WINDOWS
	/// TODO: check return code
	n_bytes = (size_t)GetModuleFileNameA(NULL, buf, (DWORD)max_size);
#elif defined(LINCE_LINUX)
	ssize_t retval = readlink("/proc/self/exe", buf, max_size);
	if(retval == -1){
		return 0;
	}
	n_bytes = (size_t)retval;
#endif
	
	// Strip filename from full path
	char* end = buf + n_bytes;
	while(*end != '/' && *end != '\\' && end != buf) end--;
	if(end == buf){
		return 0;
	}
	*(end+1) = '\0';
	return (size_t)(end - buf);
}


LinceBool LinceIsFile(const char* path){

#ifdef LINCE_WINDOWS
	// Extend path length limit from 256 to 32767
	// https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
	char long_path[1024] = {'\\','\\','?','\\'}; // use a new LINCE_PATH_MAX
	memmove(long_path + 4, path, strlen(path) + 1);
	
	// Path length limit extension only works with backward slashes
	char* p = long_path + 4;
	while(*(p++)) if(*p == '/') *p = '\\';
	
	DWORD attrib = GetFileAttributesA(path);
	return (
		attrib != INVALID_FILE_ATTRIBUTES && 
        !(attrib & FILE_ATTRIBUTE_DIRECTORY)
	);

#elif defined(LINCE_LINUX)
	struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
#endif
}


/** @brief Checks if a directory exists
* @param path Directory path
*/
LinceBool LinceIsDir(const char* path){

#ifdef LINCE_WINDOWS
	// Extend path length limit from 256 to 32767
	// https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
	char long_path[1024] = {'\\','\\','?','\\'}; // use a new LINCE_PATH_MAX
	memmove(long_path + 4, path, strlen(path) + 1);
	
	// Path length limit extension only works with backward slashes
	char* p = long_path + 4;
	while(*(p++)) if(*p == '/') *p = '\\';
	
	DWORD attrib = GetFileAttributesA(path);
	return (
		attrib != INVALID_FILE_ATTRIBUTES && 
        (attrib & FILE_ATTRIBUTE_DIRECTORY)
	);

#elif defined(LINCE_LINUX)
	struct stat path_stat;
    if(stat(path, &path_stat) != 0) return LinceFalse;
    return (path_stat.st_mode & S_IFDIR) != 0;
#endif
}


char* LinceLoadFile(const char* path){
	LINCE_INFO("Reading file '%s'", path);
	
	FILE* handle = fopen(path, "r");
	LINCE_ASSERT(handle, "Failed to open file '%s'", path);

	/* Get file length */
	fseek(handle, 0, SEEK_END);
	size_t size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	LINCE_ASSERT(size > 0, "Empty file '%s'", path);

	char* source = LinceCalloc(size*sizeof(char));
	fread(source, size, 1, handle); // load file data into buffer
	fclose(handle);

	return source;
}


char* LinceLoadTextFile(const char* path){
	LINCE_INFO("Reading text file '%s'", path);
	
	FILE* handle = fopen(path, "r");
	LINCE_ASSERT(handle, "Failed to open file '%s'", path);

	/* Get file length */
	fseek(handle, 0, SEEK_END);
	size_t size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	LINCE_ASSERT(size > 0, "Empty file '%s'", path);

	char* source = LinceCalloc((size+1)*sizeof(char));
	fread(source, size, 1, handle); // load file data into buffer
	source[size] = '\0'; // enforce last character to be terminator
	fclose(handle);

	return source;
}
