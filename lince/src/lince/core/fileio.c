#include "lince/core/core.h"
#include "lince/core/fileio.h"

#ifdef LINCE_WINDOWS
	#include "windows.h"
#elif defined(LINCE_LINUX)
	#include <unistd.h>
#endif


void LinceFetchExeDir(char* exe_path, uint64_t max_size){
	// Get location of executable
#ifdef LINCE_WINDOWS
	/// NOTE: check return code
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
}


char* LinceGetResourcePath(char* resource){


	// Append user-defined assets folders

	// For each asset folder, append asset name and check if it exists.
	// Return sucessful one, and error otherwise.
}


char* LinceLoadFile(const char* path){

}

char* LinceLoadTextFile(const char* path){
	LINCE_INFO("Reading file '%s'", path);
	
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
