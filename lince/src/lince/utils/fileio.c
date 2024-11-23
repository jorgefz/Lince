#include "lince/core/core.h"
#include "lince/core/logger.h"
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


size_t LinceFetchExecutablePath(char* buf, size_t max_size){

	size_t n_bytes = 0;
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
	while(*end != '/' && *end != '\\' && end != buf){
		end--;
	}
	if(end == buf){
		return 0;
	}

	end++; // keep last slash
	*(end+1) = '\0';
	return (size_t)(end - buf);
}


LinceBool LinceIsFile(string_t path){
	LinceBool is_file = LinceFalse;

#ifdef LINCE_WINDOWS

	// Extend path length limit from 260 to 32767 by appending \\?\ to path.
	// Only works when using GetFileAttributesW, which takes a wchar string.
	// Also requires all slashes to be backwards, e.g. "\\"
	// https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
	/*
	string_t long_path = string_from_fmt("\\\\?\\%.*s", (int)path.len, path.str);
	for(size_t i = 0; i != long_path.len; ++i){
		char* p = long_path.str + i;
		if(*p == '/') *p = '\\';
	}
	DWORD attrib = GetFileAttributesA(long_path.str);
	string_free(&long_path);
	*/
	
	DWORD attrib = GetFileAttributesA(path.str);
	is_file = (attrib != INVALID_FILE_ATTRIBUTES) && !(attrib & FILE_ATTRIBUTE_DIRECTORY);

#elif defined(LINCE_LINUX)

	struct stat path_stat;
    stat(path, &path_stat);
    is_file = S_ISREG(path_stat.st_mode);

#endif

	return is_file;
}


LinceBool LinceIsDir(string_t path){
	LinceBool is_dir = LinceFalse;

#ifdef LINCE_WINDOWS

	// Extend path length limit from 260 to 32767 by appending \\?\ to path.
	// Only works when using GetFileAttributesW, which takes a wchar string.
	// Also requires all slashes to be backwards, e.g. "\\"
	// https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
	/*
	string_t long_path = string_from_fmt("\\\\?\\%.*s", (int)path.len, path.str);
	for(size_t i = 0; i != long_path.len; ++i){
		char* p = long_path.str + i;
		if(*p == '/') *p = '\\';
	}
	DWORD attrib = GetFileAttributesA(long_path.str);
	string_free(&long_path);
	*/
	
	DWORD attrib = GetFileAttributesA(path.str);
	is_dir = attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY);

#elif defined(LINCE_LINUX)

	struct stat path_stat;
	
    if(stat(path, &path_stat) != 0){
		is_dir = LinceFalse;
	} else {
		is_dir = (path_stat.st_mode & S_IFDIR) != 0;
	}

	// if(stat(path, &path_stat) != 0) return LinceFalse;
    // return (path_stat.st_mode & S_IFDIR) != 0;

#endif

	return is_dir;
}


string_t LinceReadFile(string_t path){
	LINCE_INFO("Reading file '%s'", path.str);
	
	FILE* handle = fopen(path.str, "r");
	LINCE_ASSERT(handle, "Failed to open file '%s'", path);
	if(!handle) return (string_t){0};

	/* Get file length */
	fseek(handle, 0, SEEK_END);
	size_t size = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	LINCE_ASSERT(size > 0, "File is empty '%s'", path);
	if(size == 0){
		fclose(handle);
		return (string_t){0};
	}

	string_t contents = string_from_len(size);
	fread(contents.str, size, 1, handle); // load file data into buffer
	fclose(handle);

	return contents;
}

