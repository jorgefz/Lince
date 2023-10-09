#include "lince/core/core.h"
#include "lince/containers/array.h"
#include "lince/core/memory.h"
#include "lince/core/fileio.h"

#ifdef LINCE_WINDOWS
	#include <sys/types.h>
	#include <sys/stat.h>
	#include "windows.h"
#elif defined(LINCE_LINUX)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif


void LinceFetchExeDir(char* exe_path, uint32_t max_size){

#ifdef LINCE_WINDOWS
	/// TODO: check return code
	GetModuleFileNameA(NULL, exe_path, max_size); 
#elif defined(LINCE_LINUX)
	memset(exe_path, 0, max_size);
	LINCE_ASSERT(
		readlink("/proc/self/exe", exe_path, max_size) != -1,
		"Failed to find executable"
	);
#endif

	// Strip executable name to get directory alone
	uint64_t len = strlen(exe_path);
	char* end = exe_path + len;
	while(*end != '/' && *end != '\\' && end != exe_path) end--;
	LINCE_ASSERT(end != exe_path, "Invalid executable path '%s'", exe_path);
	*(end+1) = '\0';

	LINCE_INFO("Excutable found at '%s'", exe_path);
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
